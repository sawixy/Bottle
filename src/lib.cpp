#include "core/config/JSONConfigSystem.hpp"
#include "core/render/renderSystem.hpp"
#include "core/utils/locator.hpp"
#include <bottle.hpp>
#include <core/config/configSystem.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <iostream>

#include <core/event/eventSystem.hpp>

namespace bottle {

void Engine::init() {
    utils::Locator::Instance().add<core::config::ConfigSystem>(new core::config::JSONConfigSystem{});
    utils::Locator::Instance().get<core::config::ConfigSystem>()->load("config.json");

    utils::Locator::Instance().add<core::event::EventSystem>(new core::event::EventSystem{});
    utils::Locator::Instance().add<core::window::WindowSystem>(new core::window::glfw::GLFWWindowSystem{});
    if (utils::Locator::Instance().get<core::config::ConfigSystem>()->get<std::string>("renderer.api") == "vulkan") {
        utils::Locator::Instance().add<core::render::RenderSystem>(new core::render::vulkan::VulkanRenderSystem{});
    } else {
        throw std::runtime_error("Unsupported renderer API");
    }

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

    // TODO: Make destruction stages
    utils::Locator::Instance().get<core::event::EventSystem>()->subscribe("window_closed", [this]() {
            if (utils::Locator::Instance().get<core::config::ConfigSystem>()->get<std::string>("renderer.api") == "vulkan") {
                dynamic_cast<core::render::vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<core::render::RenderSystem>())->getRenderer().getContext().getDevice().waitIdle();
            }
            for (auto& [name, entity] : utils::Locator::Instance().getEntities()) {
                delete entity;
            }
            for (auto& [index, system] : utils::Locator::Instance().getSystems()) {
                delete system;
            }

            running = false;
        });

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