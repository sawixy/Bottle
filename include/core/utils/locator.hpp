#pragma once

#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "singleton.hpp"
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <string>

namespace bottle::utils {

class Locator : public Singleton<Locator>{
private:
    std::unordered_map<std::type_index, System*> systems;
    std::unordered_map<std::string, Entity> entities;

    Locator() = default;

public:

    /* System add, get, remove */
    template<typename T>
    void add() {
        systems[std::type_index(typeid(T))] = new T();
    }

    template<typename T>
    T* get() {
        if (systems.find(std::type_index(typeid(T))) == systems.end())
            throw std::runtime_error("System not found");

        return systems[std::type_index(typeid(T))];
    }

    template<typename T>
    void remove() {
        auto index = std::type_index(typeid(T));
        auto it = systems.find(index);
        
        if (it != systems.end()) {
            delete it->second;
            systems.erase(it);
        }
    }

    /* Entity add, get, remove */
    void add(std::string key, Entity& entity) {
        entities[key] = std::move(entity);
    }

    Entity& get(std::string key) {
        if (entities.find(key) == entities.end())
            throw std::runtime_error("Entity not found");

        return entities[key];
    }

    void remove(std::string key) {
        entities.erase(key);
    }

    ~Locator() {
        for (auto [type, value] : systems) {
            delete value;
        }
    }
};

}