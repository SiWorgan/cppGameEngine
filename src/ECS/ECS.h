#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include "Pool.h"
#include "spdlog/spdlog.h"

const unsigned int MAX_COMPONENTS = 64;

// Track of components
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

////////////////////
// Data for a component, the relevant data to be processed by a system
template <typename T>
class Component: public IComponent {
    public: 
        static int GetId(){
            static auto id = nextId++;
            return id;
        }

};



/////////////////////////////////////////////////////////////
// Entities in the game, just an ID
class Entity {
    private:
        int id;
    
    public:
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default; 
        void Kill();
        int GetId() const;

        bool operator ==(const Entity& other) const { return id == other.id; }
        bool operator !=(const Entity& other) const { return id != other.id; } 
        bool operator <(const Entity& other) const { return id < other.id; } 
        bool operator <=(const Entity& other) const { return id <= other.id; } 
        bool operator >(const Entity& other) const { return id > other.id; } 
        bool operator >=(const Entity& other) const { return id >= other.id; } 

        // Tag and Group management
        void AddTag(const std::string& tag);
        void AddGroup(const std::string& group);
        bool HasTag(const std::string& tag) const;
        bool InGroup(const std::string& group) const;
        
        // Component Management
        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        // Hold a pointer to the entity's owner registry
        class Registry* registry;
};

/////////////////////////////////////////////////////////////
// Process entities that contain specific component signatures, Collision System, AI System etc
class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;
    
    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;

        // Defines the component type that entities must have to be considered by the system
        template <typename TComponent> void RequireComponent();
};

/////////////////////////////////////////////////////////////
// Runs each system over the relevant components updating entities in the game
class Registry {
    private:
        int numEntities = 0;

        // Vector of component pools.
        // [Vector index = component type id]
        // [Pool index = entity id]
        std::vector<std::shared_ptr<IPool>> componentPools;

        // Vector of component signatures for entity, saying which component is turned "on"
        // [Vector index = entity id]
        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;

        // Entity Tags
        std::unordered_map<std::string, Entity> entityPerTag;
        std::unordered_map<int, std::string> tagPerEntity;

        // Entity Groups
        std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
        std::unordered_map<int, std::set<std::string>> groupPerEntity;

        std::deque<int> freeIds;

    public:
        Registry() {
            spdlog::info("Registry constructor called");
        }

        ~Registry() {
            spdlog::info("Registry destructor called");
        }

        Entity CreateEntity();

        void KillEntity(Entity entity);
        void Update();
        void AddEntityToSystem(Entity entity);

        // Entity Tag management
        void TagEntity(Entity entity, const std::string& tag);
        void UntagEntity(Entity entity);
        bool IsEntityTagged(Entity entity, const std::string& tag) const;
        Entity GetEntityByTag(const std::string& tag) const;

        // Entity Group management
        void AddEntityToGroup(Entity entity, const std::string& group);
        void RemoveEntityFromGroup(Entity entity);
        bool IsEntityInGroup(Entity entity, const std::string& group) const;
        std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;

        // Component management
        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity);
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;

        // System Management
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem& GetSystem() const;

        // Add & remove entity from systems according to entity signature
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);

        //TODO:
        //KillEntity();

};


///////////////////////////////////////////////////////////////////////
// Template Functions
template <typename T>
void System::RequireComponent(){
    const auto componentId = Component<T>::GetId();
    componentSignature.set(componentId);
}

//////////////////////////////////////////////////////////////
template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= static_cast<int>(componentPools.size())) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    spdlog::info("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));

}


template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);
    
    spdlog::info("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

// System Management
template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> 
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem> 
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}


//////////////////////////////////////////////////////////////
template <typename TComponent, typename ...TArgs> 
void Entity::AddComponent(TArgs&& ...args){
   registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...); 
}


template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const{
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent> 
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}


#endif