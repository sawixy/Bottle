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
    core::utils::Locator::Instance().add<core::config::ConfigSystem>(new core::config::JSONConfigSystem{});
    core::utils::Locator::Instance().get<core::config::ConfigSystem>()->load("config.json");

    core::utils::Locator::Instance().add<core::event::EventSystem>(new core::event::EventSystem{});
    core::utils::Locator::Instance().add<core::window::WindowSystem>(new core::window::glfw::GLFWWindowSystem{});
    if (core::utils::Locator::Instance().get<core::config::ConfigSystem>()->get<std::string>("renderer.api") == "vulkan") {
        core::utils::Locator::Instance().add<core::render::RenderSystem>(new core::render::vulkan::VulkanRenderSystem{});
    } else {
        throw std::runtime_error("Unsupported renderer API");
    }

    int initStage = 0;
    bool stagesLeft = true;

    while (stagesLeft) {
        stagesLeft = false;

        for (auto& [index, system] : core::utils::Locator::Instance().getSystems()) {
            if (initStage < system->getInitStages().size()) {
                (system->getInitStages()[initStage])();
                stagesLeft = true;
            }
        }
        for (auto& [name, entity] : core::utils::Locator::Instance().getEntities()) {
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
    core::utils::Locator::Instance().get<core::event::EventSystem>()->subscribe("window_closed", [this]() {
            if (core::utils::Locator::Instance().get<core::config::ConfigSystem>()->get<std::string>("renderer.api") == "vulkan") {
                dynamic_cast<core::render::vulkan::VulkanRenderSystem*>(core::utils::Locator::Instance().get<core::render::RenderSystem>())->getRenderer().getContext().getDevice().waitIdle();
            }
            for (auto& [name, entity] : core::utils::Locator::Instance().getEntities()) {
#ifdef DEBUG
                std::cout << "Destroying entity: " << name << std::endl;
#endif

                delete entity;
            }
            for (auto& [index, system] : core::utils::Locator::Instance().getSystems()) {
#ifdef DEBUG
                std::cout << "Destroying system" << std::endl;
#endif
                delete system;
            }

#ifdef DEBUG
                std::cout << "All entities and system destroyed" << std::endl;
#endif

            running = false;
        });

    while (running) {
        for (auto [index, system] : core::utils::Locator::Instance().getSystems()) {
            if (running == false) break;
#ifdef DEBUG
            std::cout << "Updating system" << std::endl;
#endif
            system->update();
        }
        for (auto [name, entity] : core::utils::Locator::Instance().getEntities()) {
            if (running == false) break;

#ifdef DEBUG
            std::cout << "Updating entity" << std::endl;
#endif

            entity->update();
        }
    }
}

}