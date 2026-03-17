#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan_raii.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace bottle::core::render::vulkan {

void Context::pickPhysicalDevice(vk::raii::PhysicalDevice& physDevice) {
    std::vector<vk::raii::PhysicalDevice> devs = instance.enumeratePhysicalDevices();

    for (auto& dev : devs) {
        vk::PhysicalDeviceFeatures feats = dev.getFeatures();
        vk::PhysicalDeviceProperties props = dev.getProperties();

        if (feats.geometryShader) {
            // TODO: Make score
            physDevice = std::move(dev);
#ifdef DEBUG
            std::cout << "Physical Device: " << props.deviceName << std::endl;
#endif
            break;
        }
    }

    if (physDevice == nullptr) {
        // TODO: Make crash
        throw std::runtime_error("Failed to pick physical device");
#ifdef DEBUG
        std::cout << "Failed to pick physical device" << std::endl;
#endif
    }

#ifdef DEBUG
    std::cout << "Physical device initialized" << std::endl;
#endif
}

void Context::createLogicalDevice(vk::raii::Device& device) {
    /* Queues */
    std::vector<vk::QueueFamilyProperties> families_props = physDevice.getQueueFamilyProperties();

    std::vector<vk::DeviceQueueCreateInfo> queuesCI{};

    float priority = 1.0f;

    for (int i = 0; i < families_props.size(); i++) {
        vk::DeviceQueueCreateInfo queueCI {
            {},
            static_cast<uint32_t>(i),
            families_props[i].queueCount,
            &priority
        };
        queuesCI.push_back(queueCI);
    }

    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physDevice.getFeatures();

    vk::DeviceCreateInfo deviceCI {
        {},
        static_cast<uint32_t>(queuesCI.size()),
        queuesCI.data(),
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data(),   // TODO: Make support check
        &physicalDeviceFeatures
    };

    device = std::move(physDevice.createDevice(deviceCI));

#ifdef DEBUG
    std::cout << "Logical device created" << std::endl;
#endif
}

}