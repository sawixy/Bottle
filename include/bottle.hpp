#pragma once

#include <core/utils/ecs/entity.hpp>
#include <core/utils/locator.hpp>
#include <core/window/windowSystem.hpp>
#include <core/render/renderSystem.hpp>

namespace bottle {

class Engine {
private:
    bool running = false;

public:
    void init();
    void run();

    void addEntity(std::string name, utils::Entity& entity) {
        utils::Locator::Instance().add(name, entity);
    }

    template<typename T>
    T* getSystem() {
        return utils::Locator::Instance().get<T>();
    }
    
};

}