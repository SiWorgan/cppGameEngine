#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
//Systems
#include "../Systems/DamageSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderCollisionSystem.h"
//Util
#include "../Utilities/TileMapLoader.h"

Game::Game() {
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
    spdlog::info("Constructor called");
}

Game::~Game(){
    spdlog::info("Destructor called");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        spdlog::error("Error initializing SDL.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    window = SDL_CreateWindow(NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS 
    );
    if (!window) {
        spdlog::error("Error creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        spdlog::error("Error creating SDL renderer.");
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        }
    }
}

void Game::LoadLevel(int level) {
    //Add the systems that need to be processed in our game
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderCollisionSystem>();
    registry->AddSystem<DamageSystem>();

    //Add assets to the asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "jungle-map", "./assets/tilemaps/jungle.png");

    //Create Entity
    Entity chopper = registry->CreateEntity();
    //Add some components
    //chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0,1.0), 0.0);
    //chopper.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0));
    //chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    //chopper.AddComponent<AnimationComponent>(2, 10, true);
    
    //Create Entity
    Entity tank = registry->CreateEntity();
    //Add some components
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0,1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    //Create Entity
    Entity truck = registry->CreateEntity();
    //Add some components
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0,1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth-74.0, windowHeight-10.0), glm::vec2(1.0,1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    //Load the map
    TileMapLoader::LoadMap("./assets/tilemaps/jungle.map", 25, 25, 32, 4.0f, registry.get(), assetStore.get());

}

void Game::Setup() {
    LoadLevel(1);
}

void Game::Update() {

    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME){
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    millisecsPreviousFrame = SDL_GetTicks();

    // Reset all event handlers
    eventBus->Reset();

    //Perform subscription of events - need a better way to do this
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);

    //invoke all the systems that need to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update(deltaTime);
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    // DamageSystem.Update();

    // Process entities waiting to be created/destroyed
    registry->Update();
}

void Game::Render() {

    // Background
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    
    registry->GetSystem<RenderSystem>().Update(renderer, *assetStore);
    registry->GetSystem<RenderCollisionSystem>().Update(renderer);

    SDL_RenderPresent(renderer);

}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}