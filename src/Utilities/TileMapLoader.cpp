#include "TileMapLoader.h"

#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

void TileMapLoader::LoadMap(const std::string& path, int mapNumRows, int mapNumCols, int tileSize, float tileScale, Registry* registry, AssetStore* assetStore) {
    std::ifstream mapFile(path);
    if (!mapFile.is_open()) {
        std::cerr << "Error: Could not open map file: " << path << "\n";
        return;
    }

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char chY, chX;
            if (!mapFile.get(chY) || !std::isdigit(chY)) continue;
            if (!mapFile.get(chX) || !std::isdigit(chX)) continue;
            mapFile.ignore();

            int srcRectY = (chY - '0') * tileSize;
            int srcRectX = (chX - '0') * tileSize;

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(
                glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                glm::vec2(tileScale, tileScale)
            );
            tile.AddComponent<SpriteComponent>("jungle-map", tileSize, tileSize,  0, srcRectX, srcRectY);
        }
    }
}
