#include <core/render/renderSystem.hpp>
#include <core/utils/locator.hpp>
#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/vulkanRenderComponent.hpp>
#include <cstring>
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

void VulkanRenderComponentInner::initBuffers() {
    indices = new Buffer(mesh.indices.size() * sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vertices = new Buffer(mesh.vertices.size() * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
}

void VulkanRenderComponentInner::loadBuffers() {
    Context& ctx = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();

    void* vdata = vertices->getMappedMemory();
    memcpy(vdata, mesh.vertices.data(), mesh.vertices.size()*sizeof(uint32_t));
    void* idata = indices->getMappedMemory();
    memcpy(idata, mesh.indices.data(), mesh.indices.size()*sizeof(uint32_t));
}

}