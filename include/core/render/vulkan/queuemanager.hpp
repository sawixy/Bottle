#pragma once

#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/render/vulkan/context.hpp>

namespace bottle::core::render::vulkan {

class QueueManager {
private:
    struct QueueFamiliesIndices {
        uint32_t graphics;
        uint32_t transfer;
        uint32_t compute;
    } indices{0, 0, 0};

    Context& ctx;

    std::vector<vk::raii::Queue> graphics{};
    std::vector<vk::raii::Queue> transfer{};
    std::vector<vk::raii::Queue> compute{};

public:
    void createQueues();

    QueueManager(Context& ctx) : ctx(ctx) {}

    enum class QueueType {
        GRAPHICS,
        TRANSFER,
        COMPUTE
    };

    vk::raii::Queue& getQueue(QueueType type, uint32_t index) {
        switch (type) {
            case QueueType::GRAPHICS: return graphics[(index < graphics.size() ? index : throw std::runtime_error("Out of boundaries"))];
            case QueueType::TRANSFER: return transfer[(index < transfer.size() ? index : throw std::runtime_error("Out of boundaries"))];
            case QueueType::COMPUTE:  return compute[(index < compute.size() ? index : throw std::runtime_error("Out of boundaries"))];
        }
    }
};

}