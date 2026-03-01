#include "doctest/doctest.hpp"

TEST_CASE("[Image] MapPixels") {
    sf::Image image({3, 3}, sf::Color::Black);
    image.setPixel({0, 0}, sf::Color::Red);
    image.setPixel({1, 1}, sf::Color::Red);
    image.setPixel({1, 0}, sf::Color::Green);
    image.setPixel({2, 2}, sf::Color::Blue);

    sf::Image mappedImage = Image::MapPixels(image, [](auto& mappedColors){
        mappedColors[sf::Color::Red.toInteger()] = sf::Color::White.toInteger();
        mappedColors[sf::Color::Green.toInteger()] = sf::Color::Cyan.toInteger();
    });

    CHECK_EQ(mappedImage.getPixel({0, 0}), sf::Color::White);
    CHECK_EQ(mappedImage.getPixel({1, 0}), sf::Color::Cyan);
    CHECK_EQ(mappedImage.getPixel({2, 0}), sf::Color::Black);
    CHECK_EQ(mappedImage.getPixel({0, 1}), sf::Color::Black);
    CHECK_EQ(mappedImage.getPixel({1, 1}), sf::Color::White);
    CHECK_EQ(mappedImage.getPixel({2, 1}), sf::Color::Black);
    CHECK_EQ(mappedImage.getPixel({0, 2}), sf::Color::Black);
    CHECK_EQ(mappedImage.getPixel({1, 2}), sf::Color::Black);
    CHECK_EQ(mappedImage.getPixel({2, 2}), sf::Color::Blue);
}