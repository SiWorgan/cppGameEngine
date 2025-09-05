#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <type_index>
#include "Pool.h"

const unsigned int MAX_COMPONENTS = 64;

// Track of components
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

template <typename T>
class Component: public IComponent {
    static int GetId(){
        static auto id = nextId++;
        return id;
    }

};

/////////////////////////////////////////////////////////////
class Entity {
    private:
        int id;
    
    public:
        Entity(int id): id(id) {}; 
        int GetId() const;

        bool operator ==(const Entity& other) const { return id == other.id; }
        bool operator !=(const Entity& other) const { return id != other.id; } 
};

/////////////////////////////////////////////////////////////
// Process entities that contain specific component signatures
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
        template <typename T> void RequireComponent();
};

/////////////////////////////////////////////////////////////
class Registry {
    private:
        int numEntities = 0;

        // Vector of component pools.
        // [Vector index = component type id]
        // [Pool index = entity id]
        std::vector<IPool*> componentPools;

        // Vector of component signatures for entity, saying which component is turned "on"
        // [Vector index = entity id]
        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, System*> systems;


    public:
        Registry() = default;

        //TODO:
        //CreateEntity();
        //KillEntity();
        //
        //AddComponent(Entity entity)
        //RemoveComponent(Entity entity)
        //HasComponent(Entity entity)
        //
        //AddSystem()
        //RemoveSystem()
        //HasSystem()
        //GetSystem()

};

template <typename T>
void System::RequireComponent(){
    const auto componentId = Component<T>::GetId();
    componentSignature.set(componentId);
}

#endif