#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/utils/locator.hpp>
#include <core/window/windowSystem.hpp>

#include <iostream>

namespace bottle::core::render::vulkan {

void Context::initInstance(vk::raii::Instance& instance) {
    vk::ApplicationInfo appinfo{
        "Bottle Game", // TODO: Application name from config
        VK_MAKE_VERSION(0, 0, 1), // Application version from config
        "Bottle",
        VK_MAKE_VERSION(0, 0, 1), // TODO: Engine version from CMake
        vk::ApiVersion14,
    };

    // TODO: Make checking extensions and layers support
    auto requiredExtensions = utils::Locator::Instance().get<window::WindowSystem>()->getRequiredVulkanExtensions();
    for (const char* ext : InstanceExtensions) {
        requiredExtensions.push_back(ext);
    }

    std::cout << "Creating Vulkan instance with the following extensions:" << std::endl;
    for (const char* ext : requiredExtensions) {
        std::cout << "  " << ext << std::endl;
    }

    const vk::InstanceCreateInfo instanceCI {
        {},
        &appinfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(requiredExtensions.size()),
        requiredExtensions.data()
    };

    instance = vk::raii::Instance(ctx, instanceCI);

    std::cout << "Instance initializated successfully" << std::endl;
}

}