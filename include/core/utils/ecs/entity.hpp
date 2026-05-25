#pragma once

#include "core/utils/ecs/component.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <functional>

namespace bottle::core::utils {

class Entity {
private:
    std::unordered_map<std::type_index, Component*> components;

protected:
    std::vector<std::function<void()>> initStages;

public:
    std::vector<std::function<void()>> getInitStages() { return initStages; }
    void addComponent(Component* component) {
        components[std::type_index(typeid(*component))] = std::move(component);
    }

    template<typename C>
    C* getComponent() {
        auto it = components.find(std::type_index(typeid(C)));
        if (it == components.end()) {
            throw std::runtime_error("Component not found");
        }
        return dynamic_cast<C*>(it->second);
    }
    virtual void update(){}

    ~Entity() {
        for (auto& [type, component] : components) {
            delete component;
        }
    }
};

}