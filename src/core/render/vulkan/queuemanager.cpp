#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/queuemanager.hpp>

namespace bottle::core::render::vulkan {

void QueueManager::createQueues() {
    std::vector<vk::QueueFamilyProperties> families_props = ctx.getPhysicalDevice().getQueueFamilyProperties();

    for (int i = 0; i < families_props.size(); i++) {
        if (families_props[i].queueFlags & vk::QueueFlags::BitsType::eGraphics) {
            indices.graphics = i;
            for (int j = 0; j < families_props[i].queueCount; j++) {
                graphics.push_back(ctx.getDevice().getQueue(i, j));
            }
        }
        if (families_props[i].queueFlags & vk::QueueFlags::BitsType::eTransfer) {
            indices.transfer = i;

            for (int j = 0; j < families_props[i].queueCount; j++) {
                transfer.push_back(ctx.getDevice().getQueue(i, j));
            }
        }
        if (families_props[i].queueFlags & vk::QueueFlags::BitsType::eCompute) {
            indices.compute = i;
            for (int j = 0; j < families_props[i].queueCount; j++) {
                compute.push_back(ctx.getDevice().getQueue(i, j));
            }
        }
    }
}

}