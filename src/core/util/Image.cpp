    #include "Image.hpp"

#include <SFML/Graphics.hpp>
#include <lodepng.h>
#include <nfd.h>

sf::Image Image::MapPixels(const sf::Image& originalImage, std::function<void(std::unordered_map<uint32_t, uint32_t>&)> mapFunc) {
    // Used for benchmarking.
    sf::Clock clock;

    std::unordered_map<uint32_t, uint32_t> mappedColors;

    // Call the mapping function to associate which colors
    // are to be replaced by which.
    mapFunc(mappedColors);

    // fmt::println("mapping colors: {}", String::DurationFormat(clock.restart()));
    // fmt::println("mapped: {} colors", mappedColors.size())

    size_t width = originalImage.getSize().x;
    size_t height = originalImage.getSize().y;
    size_t totalPixels = width * height;

    // Use vectors to avoid using SFML getters and setters for pixels.
    const uint8_t* originalPixels = originalImage.getPixelsPtr();
    std::vector<uint8_t> newPixels = std::vector<uint8_t>();
    newPixels.resize(totalPixels * 4);

    // fmt::println("image=[{}, {}]\tbytes={}", width, height, newPixels.capacity());
    // fmt::println("initializing pixels array: {}", String::DurationFormat(clock.restart()));

    const int threadsCount = 6;
    std::vector<UniquePtr<std::thread>> threads;

    // Split the image vertically between all the threads.
    // Need to be careful not to split a color from all its composites
    // in the process (so by block of 4).
    const size_t threadRange = totalPixels / threadsCount;

    for (size_t i = 0; i < threadsCount; i++) {

        threads.push_back(MakeUnique<std::thread>([&, i]() {
            size_t startIndex = i * threadRange * 4;
            size_t endIndex = (i == threadsCount - 1) ? totalPixels * 4 : (i + 1) * threadRange * 4;
            size_t index = startIndex;

            uint32_t color = 0x000000FF;
            uint32_t previousColor = 0x00000000;

            // Cast to edit directly the bytes of the color and pixels.
            // - colorPtr is used to read the color from the original image.
            // - targetPtr is used to access the color composites (RGBA)
            //   of the pixels from the array we are painting (in the new image).
            // - replacePtr is the target color to paint on the new texture.
            char* colorPtr = static_cast<char*>((void*)&color);
            char* targetPtr = static_cast<char*>((void*)&newPixels[startIndex]);
            char* replacePtr = NULL;

            while (index < endIndex) {
                // Copy the four bytes corresponding to RGBA from the original image pixels
                // to the array for the new image.
                // The bytes need to be flipped, otherwise color would be ABGR and
                // we couldn't find the associated target color in the mapped values.
                colorPtr[3] = originalPixels[index++]; // R
                colorPtr[2] = originalPixels[index++]; // G
                colorPtr[1] = originalPixels[index++]; // B
                colorPtr[0] = originalPixels[index++]; // A

                // Search for the corresponding target color in the mapped values only
                // if it isn't the same color has the previous one.
                if (previousColor != color) {
                    const auto& it = mappedColors.find(color);
                    replacePtr = (it == mappedColors.end()) ? colorPtr : static_cast<char*>((void*)&it->second);
                }

                // Replace the bits of the pixel in the new image
                // where each byte correspond to a color composite (RGBA).
                *targetPtr++ = replacePtr[3]; // R
                *targetPtr++ = replacePtr[2]; // G
                *targetPtr++ = replacePtr[1]; // B
                *targetPtr++ = replacePtr[0]; // A

                previousColor = color;
            }

            }));
    }

    for (auto& thread : threads) {
		if (thread->joinable())
            thread->join();
    }
    // fmt::println("filling pixels: {}", String::DurationFormat(clock.restart()));
    // fmt::println("initializing image: {}", String::DurationFormat(clock.restart()));

    return sf::Image({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) }, newPixels.data());
}

void Image::IndexImage(const std::string& filePath, const std::vector<sf::Color>& palette) {
    // TODO: optimize this function to avoid looping over the palette for each pixels, calling image.getPixel()...

    // Load the image to index the pixels to their respective color palette.
    sf::Image image;
    if (!image.loadFromFile(filePath)) {
        LOG_ERROR("Failed to load image '{}'", filePath);
        return;
    }

    const auto findClosestColorIndex = [&](const sf::Color& color) {
        int bestIndex = 0;
        int minDistance = INT_MAX;

        for (size_t i = 0; i < palette.size(); i++) {
            int dr = int(color.r) - int(palette[i].r);
            int dg = int(color.g) - int(palette[i].g);
            int db = int(color.b) - int(palette[i].b);
            int distance = dr * dr + dg * dg + db * db;

            if (distance < minDistance) {
                minDistance = distance;
                bestIndex = static_cast<int>(i);
            }
        }
        return bestIndex;
    };

    size_t width = image.getSize().x;
    size_t height = image.getSize().y;

    // Indexed image buffer (1 byte per pixel).
    std::vector<unsigned char> indexedPixels(width * height);

    // Map each pixel to closest palette index.
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            sf::Color pixelColor = image.getPixel(sf::Vector2u(x, y));
            int index = findClosestColorIndex(pixelColor);
            indexedPixels[y * width + x] = static_cast<unsigned char>(index);
        }
    }

    // Prepare LodePNG state.
    lodepng::State state;
    state.encoder.auto_convert = 0;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;
    lodepng_palette_clear(&state.info_raw);
    lodepng_palette_clear(&state.info_png.color);

    // Set the palette.
    for (const auto& color : palette) {
        lodepng_palette_add(&state.info_raw, color.r, color.g, color.b, color.a);
        lodepng_palette_add(&state.info_png.color, color.r, color.g, color.b, color.a);
    }

    // Encode the image.
    std::vector<unsigned char> data;
    uint32_t error = lodepng::encode(data, indexedPixels, width, height, state);
    if (error) {
        LOG_ERROR("Failed to encode image '{}': {}", filePath, lodepng_error_text(error));
        return;
    }

    // Save PNG
    error = lodepng::save_file(data, filePath);
    if (error) {
        LOG_ERROR("Failed to save image '{}': {}", filePath, lodepng_error_text(error));
        return;
    }
}

void Image::SaveWithDialog(const sf::Image& image) {
    nfdchar_t* outPath = nullptr;

    nfdresult_t result = NFD_SaveDialog(
        "png",
        "image.png",
        &outPath
    );

    if (result == NFD_OKAY) {
        std::string pathStr(outPath);
        if (!pathStr.ends_with(".png")) {
            pathStr += ".png";
        }
        image.saveToFile(pathStr);
        free(outPath);
    }
    else if (result == NFD_ERROR) {
        throw std::runtime_error(fmt::format("Failed to save image: {}", NFD_GetError()));
    }
}