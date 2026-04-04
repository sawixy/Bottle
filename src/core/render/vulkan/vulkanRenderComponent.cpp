#include <core/render/renderSystem.hpp>
#include <core/utils/locator.hpp>
#include "core/render/vulkan/queuemanager.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/vulkanRenderComponent.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/vulkanRenderSystem.hpp>

namespace bottle::core::render::vulkan {

uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, 
                         uint32_t typeFilter, 
                         vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();
    
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type");
}

void VulkanRenderComponent::initBuffers() {
    int graphics = static_cast<uint32_t>(dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getQueueManager().getFamilyIndex(QueueManager::QueueType::GRAPHICS));
    vk::BufferCreateInfo vbufCI {
        {},
        mesh.vertices.size() * sizeof(Vertex),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::SharingMode::eExclusive,
        1,
        reinterpret_cast<uint32_t*>(&graphics),
        nullptr
    };

    Context& ctx = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();
    vertices = ctx.getDevice().createBuffer(vbufCI);

    vk::BufferCreateInfo ibufCI {
        {},
        mesh.indices.size() * sizeof(uint32_t),
        vk::BufferUsageFlagBits::eIndexBuffer,
        vk::SharingMode::eExclusive,
        1,
        reinterpret_cast<uint32_t*>(&graphics),
        nullptr
    };

    indices = ctx.getDevice().createBuffer(ibufCI);

    // allocating memory
    vk::MemoryRequirements memreq = vertices.getMemoryRequirements();

    uint32_t memory_type = findMemoryType(ctx.getPhysicalDevice(), memreq.memoryTypeBits,  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::MemoryAllocateInfo allocI {
        memreq.size,
        memory_type,
        nullptr
    };
    verticesMemory = ctx.getDevice().allocateMemory(allocI);
    vertices.bindMemory(*verticesMemory, 0);

    indicesMemory = ctx.getDevice().allocateMemory(allocI);
    indices.bindMemory(*indicesMemory, 0);
}

void VulkanRenderComponent::loadBuffers() {
    Context& ctx = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();
}

}