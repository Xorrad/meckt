#include "Math.hpp"

int Math::RandomInt(int min, int max) {
    return min + static_cast<int>((static_cast<double>(rand()) / RAND_MAX) * (max-min));
}

float Math::RandomFloat(float min, float max) {
    return min + (((float) rand()) / (float) RAND_MAX) * (max-min);
}

bool Math::IsInt(const std::string& str) {
    if (str.empty())
        return false;
    size_t i = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (str.size() == 1)
            return false;
        i = 1;
    }
    for (; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9')
            return false;
    }
    return true;
}