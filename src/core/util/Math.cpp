#include "Math.hpp"

int Math::RandomInt(int min, int max) {
    return min + static_cast<int>((static_cast<double>(rand()) / RAND_MAX) * (max-min));
}

float Math::RandomFloat(float min, float max) {
    return min + (((float) rand()) / (float) RAND_MAX) * (max-min);
}

bool Math::IsInt(const std::string& str) {
    for (char ch : str) {
        if (ch < '0' || ch > '9')
            return false;
    }
    return true;
}