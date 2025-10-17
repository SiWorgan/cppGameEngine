#ifndef TILEMAPLOADER_H
#define TILEMAPLOADER_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"

class TileMapLoader {
public:
    static void LoadMap(
        const std::string& path,
        int mapNumRows,
        int mapNumCols,
        int tileSize,
        float tileScale,
        Registry* registry,
        AssetStore* assetStore
    );
};

#endif