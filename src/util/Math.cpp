#include "Math.hpp"

int Math::RandomInt(int min, int max) {
    return min + (rand() / RAND_MAX) * (max-min);
}

float Math::RandomFloat(float min, float max) {
    return min + (((float) rand()) / (float) RAND_MAX) * (max-min);
}