#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

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

    const vk::InstanceCreateInfo instanceCI {
        {},
        &appinfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data()
    };

    instance = vk::raii::Instance(ctx, instanceCI);

#ifdef DEBUG
    std::cout << "Instance initializated successfully" << std::endl;
#endif
}

}