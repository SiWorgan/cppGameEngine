#include "AssetStore.h"
#include "spdlog/spdlog.h"
#include <SDL2/SDL_image.h>
#include <fstream>

AssetStore::AssetStore(){
    spdlog::info("AssetStore constructor called");
}

AssetStore::~AssetStore(){
    ClearAssets();
}

void AssetStore::ClearAssets() {
    spdlog::info("Asset Store Destructor called");
    for (auto texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

//void AssetStore::AddTiles(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath, const int tileSize = 32) {
//    std::ifstream tilemapFile;
//    tilemapFile.open(filePath);
//    std::vector<int> tilemap;
//
//    int tilemapWidth, tilemapHeight;
//    SDL_Texture* tilemapTexture = this->GetTexture(assetId);
//    SDL_QueryTexture(tilemapTexture, NULL, NULL, &tilemapWidth, &tilemapHeight);
//
//    int tilesPerRow = tilemapWidth / tileSize;
//    int tilesPerCol = tilemapHeight / tileSize;
//
//    for (int y = 0; y < tilesPerRow; y++) {
//        for (int x = 0; x < tilesPerCol; x++) {
//            
//        }
//    }
//
//    tilemapFile.close();
//    
//}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    //Add texture to map
    textures.emplace(assetId, texture);

    spdlog::info("New texture added to Asset Store: "  + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId){
    //TODO: Need to throw warning when assetId not in texture store 
    return textures[assetId];
}