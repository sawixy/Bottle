#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/queuemanager.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "core/utils/locator.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/buffer.hpp>

namespace bottle::core::render::vulkan {

uint32_t Buffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto memProperties = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext().getPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

Buffer::Buffer(size_t size, vk::BufferUsageFlags usage) : size(size){
    Context& ctx = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();
    uint32_t graphicsQueueFamilyIndex = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getQueueManager().getFamilyIndex(QueueManager::QueueType::GRAPHICS);

    vk::BufferCreateInfo bufferCI {
        {},
        size,
        usage, 
        vk::SharingMode::eExclusive,
        1,
        &graphicsQueueFamilyIndex
    };

    buffer = std::move(ctx.getDevice().createBuffer(bufferCI));

    auto memRequirements = buffer.getMemoryRequirements();

    vk::MemoryAllocateInfo allocInfo {
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) // TODO: Make this more flexible
    };

    memory = std::move(ctx.getDevice().allocateMemory(allocInfo));

    vk::BindBufferMemoryInfo bindInfo {
        buffer,
        memory,
        0,
        nullptr
    };
    ctx.getDevice().bindBufferMemory2(bindInfo);
}

void* Buffer::getMappedMemory() {
    return memory.mapMemory(0, size);
}

Buffer::~Buffer() {
    memory.unmapMemory();
}

}