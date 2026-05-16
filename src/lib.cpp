#include "core/render/renderSystem.hpp"
#include "core/utils/locator.hpp"
#include <bottle.hpp>
#include <core/config/configSystem.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>

namespace bottle {

void Engine::init() {
    utils::Locator::Instance().add<core::config::ConfigSystem>(new core::config::ConfigSystem{});
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
    static bool running = true;
    while (running) {
        for (auto [index, system] : utils::Locator::Instance().getSystems()) {
            system->update();
        }
        for (auto [name, entity] : utils::Locator::Instance().getEntities()) {
            entity->update();
        }
    }
}

}