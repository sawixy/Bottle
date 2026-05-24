#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/uniform.hpp>
#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/utils/locator.hpp>

namespace bottle::core::render {

void Uniform::initBuffer() {
    auto& ctx = dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();

    const vk::DeviceSize bufferSize = last_offset;

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCI {{}, 1, &binding};
    setLayout = std::move(ctx.getDevice().createDescriptorSetLayout(descSetLayoutCI));

    vk::DescriptorPoolSize poolSize {vk::DescriptorType::eUniformBuffer, 1};
    vk::DescriptorPoolCreateInfo descPoolCI {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, 1, &poolSize};
    pool = std::move(ctx.getDevice().createDescriptorPool(descPoolCI));

    vk::BufferCreateInfo bufferInfo {{}, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive};
    buf = vk::raii::Buffer(ctx.getDevice(), bufferInfo);

    auto memReqs = buf.getMemoryRequirements();
    auto memProps = ctx.getPhysicalDevice().getMemoryProperties();

    uint32_t memTypeIndex = ~0u;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReqs.memoryTypeBits & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags &
             (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))) {
            memTypeIndex = i;
            break;
        }
    }

    vk::MemoryAllocateInfo allocInfo {memReqs.size, memTypeIndex};
    mem = vk::raii::DeviceMemory(ctx.getDevice(), allocInfo);
    buf.bindMemory(*mem, 0);

    data = mem.mapMemory(0, bufferSize);
    memset(data, 0, bufferSize);

    vk::DescriptorSetAllocateInfo descSetAllocI {*pool, 1, &*setLayout};
    descSet = std::move(ctx.getDevice().allocateDescriptorSets(descSetAllocI)[0]);

    vk::DescriptorBufferInfo bufferDescInfo {*buf, 0, bufferSize};
    vk::WriteDescriptorSet write {*descSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferDescInfo};
    ctx.getDevice().updateDescriptorSets(write, nullptr);
}


void Uniform::set(std::string name, uint value) {
    *reinterpret_cast<uint*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, int value) {
    *reinterpret_cast<int*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, float value) {
    *reinterpret_cast<float*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, glm::vec2 value) {
    *reinterpret_cast<glm::vec2*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, glm::vec3 value) {
    *reinterpret_cast<glm::vec3*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, glm::mat3 value) {
    *reinterpret_cast<glm::mat3*>(static_cast<char*>(data) + map[name].offset) = value;
}

void Uniform::set(std::string name, glm::mat4 value) {
    *reinterpret_cast<glm::mat4*>(static_cast<char*>(data) + map[name].offset) = value;
}

}