#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/queuemanager.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/uniform.hpp>
#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/utils/locator.hpp>

namespace bottle::core::render {

void Uniform::initBuffer() {
    auto& ctx = dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();
    auto& queueManager = dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getQueueManager();

    vk::DescriptorPoolSize size {
        vk::DescriptorType::eUniformBuffer,
        1
    };

    vk::DescriptorPoolCreateInfo descPoolCI {
        {},
        1000,
        1,
        &size
    };
    pool = std::move(ctx.getDevice().createDescriptorPool(descPoolCI));

    const vk::DeviceSize bufferSize = last_offset;

    uint32_t graphics = queueManager.getFamilyIndex(vulkan::QueueManager::QueueType::GRAPHICS);
    vk::BufferCreateInfo bufferInfo{
        {},
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        1,
        &graphics
    };
    buf = vk::raii::Buffer(ctx.getDevice(), bufferInfo);

    auto memReqs = buf.getMemoryRequirements();
    auto memProps = ctx.getPhysicalDevice().getMemoryProperties();

    uint32_t memTypeIndex = ~0u;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReqs.memoryTypeBits & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags &
             (vk::MemoryPropertyFlagBits::eHostVisible |
              vk::MemoryPropertyFlagBits::eHostCoherent))) {
            memTypeIndex = i;
            break;
        }
    }

    vk::MemoryAllocateInfo allocInfo{
        bufferSize,
        0,
        nullptr
    };
    mem = vk::raii::DeviceMemory(ctx.getDevice(), allocInfo);
    buf.bindMemory(*mem, 0);

    data = mem.mapMemory(0, bufferSize);
    memset(data, 0, bufferSize);

    vk::DescriptorSetAllocateInfo descSetAllocI {
        pool,
        1,
        &*setLayout,
        nullptr
    };
    descSet = std::move(ctx.getDevice().allocateDescriptorSets(descSetAllocI)[0]);

    vk::DescriptorBufferInfo bufferDescInfo{
        buf,
        last_offset,
        0
    };

    vk::WriteDescriptorSet write{
        descSet,
        0,
        0,
        0,
        vk::DescriptorType::eUniformBuffer,
        nullptr,
        &bufferDescInfo,
        nullptr
    };

    ctx.getDevice().updateDescriptorSets(write, nullptr);

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCI {
        {},
        1,
        &binding,
        nullptr
    };
    setLayout = std::move(ctx.getDevice().createDescriptorSetLayout(descSetLayoutCI));
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