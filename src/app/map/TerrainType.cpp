#include "TerrainType.hpp"

TerrainType TerrainTypefromString(std::string str) {
    for(int i = 0; i < TerrainTypeLabels.size(); i++) {
        if(String::ToLowercase(TerrainTypeLabels[i]) == str)
            return (TerrainType) i;
    }
    return TerrainType::PLAINS;
}