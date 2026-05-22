#include "core/config/JSONConfigSystem.hpp"
#include "core/render/renderSystem.hpp"
#include "core/utils/locator.hpp"
#include <bottle.hpp>
#include <core/config/configSystem.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <iostream>

namespace bottle {

void Engine::quit() {
    running = false;
}


void Engine::init() {
    utils::Locator::Instance().add<core::config::ConfigSystem>(new core::config::JSONConfigSystem{});
    utils::Locator::Instance().get<core::config::ConfigSystem>()->load("config.json");

    utils::Locator::Instance().add<core::window::WindowSystem>(new core::window::glfw::GLFWWindowSystem{});
    utils::Locator::Instance().add<core::render::RenderSystem>(new core::render::vulkan::VulkanRenderSystem{});

    int initStage = 0;
    bool stagesLeft = true;

    while (stagesLeft) {
        stagesLeft = false;

        for (auto& [index, system] : utils::Locator::Instance().getSystems()) {
            if (initStage < system->getInitStages().size()) {
                (system->getInitStages()[initStage])();
                stagesLeft = true;
            }
        }
        for (auto& [name, entity] : utils::Locator::Instance().getEntities()) {
            if (initStage < entity->getInitStages().size()) {
                (entity->getInitStages()[initStage])();
                stagesLeft = true;
            }
        }

        initStage++;
    }
}

void Engine::run() {
    running = true;
    while (running) {
        try {
        for (auto [index, system] : utils::Locator::Instance().getSystems()) {
            system->update();
        }
        for (auto [name, entity] : utils::Locator::Instance().getEntities()) {
            entity->update();
        }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in main loop: " << e.what() << std::endl;
            quit();
        }
    }
    delete utils::Locator::Instance().get<core::render::RenderSystem>();
    delete utils::Locator::Instance().get<core::window::WindowSystem>();
    delete utils::Locator::Instance().get<core::config::ConfigSystem>();
}

}