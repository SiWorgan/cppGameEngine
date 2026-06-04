#include "ECS.h"
#include "spdlog/spdlog.h"

#include <algorithm>

int IComponent::nextId = 0;

// Entity
int Entity::GetId() const {
    return id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void Entity::AddTag(const std::string& tag) {
    registry->TagEntity(*this, tag);
}

void Entity::AddGroup(const std::string& group) {
    registry->AddEntityToGroup(*this, group);
}

bool Entity::HasTag(const std::string& tag) const {
    return registry->IsEntityTagged(*this, tag);
}

bool Entity::InGroup(const std::string& group) const {
    return registry->IsEntityInGroup(*this, group);
}


// System
void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other){
        return entity == other;
    }), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}


//////////////////////////////////////////////
Entity Registry::CreateEntity() {
    int entityId;

    if (freeIds.empty()) {
        entityId = numEntities++;
        if (entityId >= static_cast<int>(entityComponentSignatures.size())) {
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // Reuse a previous id
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    spdlog::info("Entity created with id = " + std::to_string(entityId));
    return entity;
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();
    const auto& entityComponentSignature = entityComponentSignatures[entityId];
    for (auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto system: systems) {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::TagEntity(Entity entity, const std::string& tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

void Registry::UntagEntity(Entity entity) {
    auto taggedEntity = tagPerEntity.find(entity.GetId());
    if (taggedEntity != tagPerEntity.end()) {
        entityPerTag.erase(taggedEntity->second);
        tagPerEntity.erase(taggedEntity);
    }
}

// Check this works
bool Registry::IsEntityTagged(Entity entity, const std::string& tag) const {
    auto taggedEntity = entityPerTag.find(tag);
    return taggedEntity != entityPerTag.end() && taggedEntity->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) const {
    return entityPerTag.at(tag);
}

void Registry::AddEntityToGroup(Entity entity, const std::string& group) {
    entitiesPerGroup[group].insert(entity);
    groupPerEntity[entity.GetId()].insert(group);
}

bool Registry::IsEntityInGroup(Entity entity, const std::string& group) {
    auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity.GetId()) != groupEntites.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
    auto &setOfEntities = entitiesPerGroup.at(group);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void Registry::RemoveEntityFromGroup(Entity entity) {
    auto groupedEntity = groupPerEntity.find(entity.GetId());
    if (groupedEntity != groupPerEntity.end()) {
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if (group != entitiesPerGroup.end()) {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end()) {
                group -> second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

void Registry::Update() {
    // Add the entities that are waiting to be added
    for (auto entity: entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    // Kill the entities that are waiting to be killed
    for (auto entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);

        entityComponentSignatures[entity.GetId()].reset();
        //Make the entity id available to be reused
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeKilled.clear();
}