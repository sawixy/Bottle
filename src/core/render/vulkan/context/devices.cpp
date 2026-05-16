#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>
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

    float* priorities = new float[families_props.size()];

    for (int i = 0; i < families_props.size(); i++) {
        priorities[i] = 1.0f;
    }

    for (int i = 0; i < families_props.size(); i++) {
        vk::DeviceQueueCreateInfo queueCI {
            {},
            static_cast<uint32_t>(i),
            families_props[i].queueCount,
            priorities,
            nullptr
        };
        queuesCI.push_back(queueCI);
    }

    vk::PhysicalDeviceFeatures physicalDeviceFeatures = physDevice.getFeatures();

    vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures {
        vk::True,
        nullptr
    };

    auto availableExtensions = physDevice.enumerateDeviceExtensionProperties();
    std::vector<const char*> enabledExtensions;
    enabledExtensions.reserve(DeviceExtensions.size());

    for (auto ext : DeviceExtensions) {
        bool supported = false;
        for (const auto& available : availableExtensions) {
            if (std::strcmp(available.extensionName, ext) == 0) {
                supported = true;
                break;
            }
        }

        if (supported) {
            enabledExtensions.push_back(ext);
        }
#ifdef DEBUG
        else {
            std::cout << "Device extension not supported: " << ext << std::endl;
        }
#endif
    }

    if (std::find(enabledExtensions.begin(), enabledExtensions.end(), "VK_KHR_swapchain") == enabledExtensions.end()) {
        throw std::runtime_error("Required device extension VK_KHR_swapchain is not supported");
    }

    vk::DeviceCreateInfo deviceCI {
        {},
        static_cast<uint32_t>(queuesCI.size()),
        queuesCI.data(),
        0,
        nullptr,
        static_cast<uint32_t>(enabledExtensions.size()),
        enabledExtensions.data(),
        &physicalDeviceFeatures,
        &dynamicRenderingFeatures
    };

    device = std::move(physDevice.createDevice(deviceCI));

#ifdef DEBUG
    std::cout << "Logical device created" << std::endl;
#endif
}

}