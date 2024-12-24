#ifndef ECS_H
#define ECS_H

#include <unordered_map>
#include <bitset>
#include <vector>
#include <memory>
#include <iostream>

// Maximum number of components that an entity can have
const size_t MAX_COMPONENTS = 32;

// Type alias for entity identifiers and component types
using Entity = uint32_t;
using ComponentType = uint8_t;

// Base interface for all components
struct IComponent
{
    virtual ~IComponent() = default;
};

// Unique identifier for each component type
using ComponentID = std::size_t;

// Generates a unique ID for each component type
inline ComponentID getUniqueComponentID()
{
    static ComponentID lastID = 0;
    return lastID++;
}

// Template function to get a unique component type ID for each component class
template <typename T>
inline ComponentID getComponentTypeID() noexcept
{
    static ComponentID typeID = getUniqueComponentID();
    return typeID;
}

// A bitset to represent an entity's signature (which components it has)
using Signature = std::bitset<MAX_COMPONENTS>;

// Manages entity creation
class EntityManager
{
public:
    // Creates and returns a new entity
    Entity createEntity()
    {
        Entity entity = nextEntity++;
        return entity;
    }

private:
    // Tracks the next entity ID to assign
    Entity nextEntity = 0;
};

// Base interface for all systems
class ISystem
{
public:
    virtual ~ISystem() = default;
    virtual void update(double deltaTime) = 0;
};

// Manages systems, allowing for registration and updating
class SystemManager
{
public:
    // Registers a new system and returns a shared pointer to it
    template <typename T>
    std::shared_ptr<T> registerSystem()
    {
        auto system = std::make_shared<T>();
        systems.emplace_back(system);
        return system;
    }

    // Calls update on all registered systems
    void updateSystems(double deltaTime)
    {
        for (auto &system : systems)
        {
            system->update(deltaTime);
        }
    }

private:
    // Stores all registered systems
    std::vector<std::shared_ptr<ISystem>> systems;
};

// Manages component types and their data storage
class ComponentManager
{
public:
    // Registers a component type and creates its storage array
    template <typename T>
    void registerComponent()
    {
        const char *typeName = typeid(T).name();
        componentTypes.insert({typeName, getComponentTypeID<T>()});
        componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
    }

    // Adds a component to an entity
    template <typename T>
    void addComponent(Entity entity, T component)
    {
        getComponentArray<T>()->insertData(entity, component);
    }

    // Retrieves a component from an entity
    template <typename T>
    T &getComponent(Entity entity)
    {
        return getComponentArray<T>()->getData(entity);
    }

    // Removes a component from an entity
    template <typename T>
    void removeComponent(Entity entity)
    {
        getComponentArray<T>()->removeData(entity);
    }

private:
    // Maps component type names to their unique IDs
    std::unordered_map<const char *, ComponentID> componentTypes;

    // Base interface for component arrays
    struct IComponentArray
    {
        virtual ~IComponentArray() = default;
    };

    // Template class to store components of a specific type
    template <typename T>
    class ComponentArray : public IComponentArray
    {
    public:
        // Inserts a component for an entity
        void insertData(Entity entity, T component)
        {
            entityToIndexMap[entity] = size;
            indexToEntityMap[size] = entity;
            componentArray[size] = component;
            ++size;
        }

        // Removes a component for an entity
        void removeData(Entity entity)
        {
            size_t indexOfRemovedEntity = entityToIndexMap[entity];
            size_t indexOfLastElement = size - 1;
            componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

            Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
            entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            entityToIndexMap.erase(entity);
            indexToEntityMap.erase(indexOfLastElement);
            --size;
        }

        // Retrieves a component for an entity
        T &getData(Entity entity)
        {
            return componentArray[entityToIndexMap[entity]];
        }

    private:
        // Array to store components (arbitrary size, adjust as needed)
        std::array<T, 1000> componentArray;
        // Maps entity IDs to their component array index
        std::unordered_map<Entity, size_t> entityToIndexMap;
        // Maps array indices to entity IDs
        std::unordered_map<size_t, Entity> indexToEntityMap;
        // Tracks the current size of the component array
        size_t size = 0;
    };

    // Maps component type names to their respective component arrays
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>> componentArrays;

    // Retrieves the component array for a specific component type
    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray()
    {
        const char *typeName = typeid(T).name();
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
    }
};

#endif // ECS_H
