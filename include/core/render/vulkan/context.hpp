#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>
#include <vector>

namespace bottle::core::render::vulkan {

/* Layers */
const std::vector<const char*> layers{
    "VK_LAYER_KHRONOS_validation"
};

/* Exstensions */
const std::vector<const char*> InstanceExtensions {
};

const std::vector<const char*> DeviceExtensions {
    "VK_KHR_swapchain",
    "VK_KHR_dynamic_rendering"
};

class Context {
private:
    vk::raii::Context ctx;
    vk::raii::Instance instance{nullptr};
    vk::raii::PhysicalDevice physDevice{nullptr};
    vk::raii::Device device{nullptr};
    std::vector<vk::raii::Queue> queues;

    struct QueueIndices {
        int graphics;
        int compute;
        int transfer;
    } queueIndices;

public:
    Context() {
        initInstance(instance);
        pickPhysicalDevice(physDevice);
        createLogicalDevice(device);
    }

    /* Initialization */
    void initInstance(vk::raii::Instance& instance);
    void pickPhysicalDevice(vk::raii::PhysicalDevice& physDevice);
    void createLogicalDevice(vk::raii::Device& device);

    /* Getters */
    const vk::raii::Instance& getInstance() const { return instance; }
    const vk::raii::PhysicalDevice& getPhysicalDevice() const { return physDevice; }
    const vk::raii::Device& getDevice() const { return device; }
};

}