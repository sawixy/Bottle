#include "core/render/renderSystem.hpp"
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/utils/locator.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>

int main() {
    try {
        vk::detail::DynamicLoader dl;
        auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    } catch (const std::exception& e) {
        return -1;
    }
    
    bottle::utils::Locator::Instance().add<bottle::core::window::WindowSystem>(new bottle::core::window::glfw::GLFWWindowSystem{});
    bottle::utils::Locator::Instance().add<bottle::core::render::RenderSystem>(new bottle::core::render::vulkan::VulkanRenderSystem{});

    auto r = bottle::utils::Locator::Instance().get<bottle::core::render::RenderSystem>();
    auto w = bottle::utils::Locator::Instance().get<bottle::core::window::WindowSystem>();

    while (1) {
        r->update();
        w->update();
    }
    
    return 0;
}