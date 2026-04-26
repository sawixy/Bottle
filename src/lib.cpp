#include "core/render/renderSystem.hpp"
#include "core/utils/locator.hpp"
#include <bottle.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>

namespace bottle {

void Engine::init() {
    utils::Locator::Instance().add<core::window::WindowSystem>(new core::window::glfw::GLFWWindowSystem{});
    utils::Locator::Instance().add<core::render::RenderSystem>(new core::render::vulkan::VulkanRenderSystem{});

    for (auto [name, entity] : utils::Locator::Instance().getEntities()) {
        entity->onStart();
    }

    for (auto [name, system] : utils::Locator::Instance().getSystems()) {
        system->pastInit();
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