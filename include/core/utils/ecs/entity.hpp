#pragma once

#include <typeindex>
#include <unordered_map>
#include <any>

namespace bottle::utils {

class Entity {
private:
    std::unordered_map<std::type_index, std::any> components;

public:
    void addComponent(std::any component);
    template<typename C>
    C& getComponent();
};

}