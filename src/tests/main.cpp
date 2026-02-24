#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.hpp"

int main(int argc, char** argv) {
    doctest::Context context(argc, argv);
    context.run();
    return 0;
}