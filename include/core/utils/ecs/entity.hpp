#pragma once

#include <typeindex>
#include <unordered_map>
#include <any>
#include <stdexcept>

namespace bottle::utils {

class Entity {
private:
    std::unordered_map<std::type_index, std::any> components;

public:
    void addComponent(std::any component) {
        components[std::type_index(component.type())] = std::move(component);
    }

    template<typename C>
    C& getComponent() {
        auto it = components.find(std::type_index(typeid(C)));
        if (it == components.end()) {
            throw std::runtime_error("Component not found");
        }
        return std::any_cast<C&>(it->second);
    }

    virtual void onStart(){}
    virtual void update(){}
};

}