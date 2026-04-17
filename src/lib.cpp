#include "core/render/renderSystem.hpp"
#include "core/utils/locator.hpp"
#include <bottle.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>

namespace bottle {

void Engine::init() {
    utils::Locator::Instance().add<core::window::WindowSystem>(new core::window::glfw::GLFWWindowSystem{});
    utils::Locator::Instance().add<core::render::RenderSystem>(new core::render::vulkan::VulkanRenderSystem{});
}

void Engine::run() {
    static bool running = true;
    while (running) {
        utils::Locator::Instance().get<core::window::WindowSystem>()->update();
        utils::Locator::Instance().get<core::render::RenderSystem>()->update();
    }
}

}