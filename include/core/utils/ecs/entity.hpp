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
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

protected:
    std::vector<std::function<void()>> initStages;

public:
    std::vector<std::function<void()>> getInitStages() { return initStages; }
    void addComponent(std::unique_ptr<Component> component) {
        components[std::type_index(typeid(*component))] = std::move(component);
    }

    template<typename C>
    std::unique_ptr<C> getComponent() {
        auto it = components.find(std::type_index(typeid(C)));
        if (it == components.end()) {
            throw std::runtime_error("Component not found");
        }
        return it->second;
    }
    virtual void update(){}
};

}