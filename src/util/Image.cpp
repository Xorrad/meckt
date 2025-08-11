#include "Image.hpp"

#include <SFML/Graphics.hpp>
#include "lodepng/lodepng.h"

sf::Image Image::MapPixels(const sf::Image& originalImage, std::function<void(std::unordered_map<sf::Uint32, sf::Uint32>&)> mapFunc) {
    // Used for benchmarking.
    sf::Clock clock;

    sf::Image image;
    std::unordered_map<sf::Uint32, sf::Uint32> mappedColors;

    // Call the mapping function to associate which colors
    // are to be replaced by which.
    mapFunc(mappedColors);

    // fmt::println("mapping colors: {}", String::DurationFormat(clock.restart()));
    // fmt::println("mapped: {} colors", mappedColors.size())

    uint width = originalImage.getSize().x;
    uint height = originalImage.getSize().y;
    uint totalPixels = width * height;

    // Use vectors to avoid using SFML getters and setters for pixels.
    const sf::Uint8* originalPixels = originalImage.getPixelsPtr();
    std::vector<sf::Uint8> newPixels = std::vector<sf::Uint8>();
    newPixels.reserve(totalPixels * 4);

    // fmt::println("image=[{}, {}]\tbytes={}", width, height, newPixels.capacity());
    // fmt::println("initializing pixels array: {}", String::DurationFormat(clock.restart()));

    const int threadsCount = 6;
    std::vector<UniquePtr<sf::Thread>> threads;

    // Split the image vertically between all the threads.
    // Need to be careful not to split a color from all its composites
    // in the process (so by block of 4).
    const uint threadRange = totalPixels / threadsCount;

    for(uint i = 0; i < threadsCount; i++) {

        threads.push_back(MakeUnique<sf::Thread>([&, i](){
            uint startIndex = i * threadRange*4;
            uint endIndex = (i == threadsCount-1) ? totalPixels*4 : (i+1) * threadRange*4;
            uint index = startIndex;

            sf::Uint32 color = 0x000000FF;
            sf::Uint32 previousColor = 0x00000000;

            // Cast to edit directly the bytes of the color and pixels.
            // - colorPtr is used to read the color from the original image.
            // - targetPtr is used to access the color composites (RGBA)
            //   of the pixels from the array we are painting (in the new image).
            // - replacePtr is the target color to paint on the new texture.
            char* colorPtr = static_cast<char*>((void*) &color);
            char* targetPtr = static_cast<char*>((void*) &newPixels[startIndex]);
            char* replacePtr = NULL;

            while(index < endIndex) {
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
                if(previousColor != color) {
                    const auto& it = mappedColors.find(color);
                    replacePtr = (it == mappedColors.end()) ? colorPtr : static_cast<char*>((void*) &it->second);
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
        threads[threads.size()-1]->launch();
    }

    for(auto& thread : threads)
        thread->wait();
    // fmt::println("filling pixels: {}", String::DurationFormat(clock.restart()));

    image.create(width, height, newPixels.data());
    // fmt::println("initializing image: {}", String::DurationFormat(clock.restart()));

    return image;
}

void Image::IndexImage(std::filesystem::path filePath, const std::vector<sf::Color>& palette) {
    // TODO: optimize this function to avoid looping over the palette for each pixels, calling image.getPixel()...

    // Load the image to index the pixels to their respective color palette.
    sf::Image image;
    if (!image.loadFromFile(filePath)) {
        LOG_ERROR("Failed to load image '{}'", filePath.c_str());
        return;
    }

    const auto findClosestColorIndex = [&](const sf::Color& color) {
        int bestIndex = 0;
        int minDistance = INT_MAX;

        for (size_t i = 0; i < palette.size(); i++) {
            int dr = int(color.r) - int(palette[i].r);
            int dg = int(color.g) - int(palette[i].g);
            int db = int(color.b) - int(palette[i].b);
            int da = int(color.a) - int(palette[i].a);
            int distance = dr * dr + dg * dg + db * db + da * da;

            if (distance < minDistance) {
                minDistance = distance;
                bestIndex = static_cast<int>(i);
            }
        }
        return bestIndex;
    };

    uint width = image.getSize().x;
    uint height = image.getSize().y;

    // Indexed image buffer (1 byte per pixel).
    std::vector<unsigned char> indexedPixels(width * height);

    // Map each pixel to closest palette index.
    for (uint y = 0; y < height; y++) {
        for (uint x = 0; x < width; x++) {
            sf::Color pixelColor = image.getPixel(x, y);
            int index = findClosestColorIndex(pixelColor);
            indexedPixels[y * width + x] = static_cast<unsigned char>(index);
        }
    }

    // Prepare LodePNG state.
    lodepng::State state;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;
    lodepng_palette_clear(&state.info_png.color);

    // Set the palette.
    for (const auto& color : palette) {
        lodepng_palette_add(&state.info_png.color, color.r, color.g, color.b, color.a);
    }

    // Encode the image.
    std::vector<unsigned char> data;
    uint error = lodepng::encode(data, indexedPixels, width, height, state);
    if (error) {
        LOG_ERROR("Failed to encode image '{}': {}", filePath.c_str(), lodepng_error_text(error));
        return;
    }

    // Save PNG
    error = lodepng::save_file(data, filePath);
    if (error) {
        LOG_ERROR("Failed to save image '{}': {}", filePath.c_str(), lodepng_error_text(error));
        return;
    }
}