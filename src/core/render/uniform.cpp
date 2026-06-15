#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/buffer.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/uniform.hpp>
#include <core/render/vulkan/context.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/utils/locator.hpp>

namespace bottle::core::render {

void Uniform::initBuffer() {
    auto& ctx = dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();

    vk::DescriptorSetLayoutCreateInfo layoutCI {
        {},
        1,
        &binding
    };
    setLayout = vk::raii::DescriptorSetLayout(ctx.getDevice(), layoutCI);

    vk::DescriptorPoolSize poolSize {
        vk::DescriptorType::eUniformBuffer,
        1
    };
    vk::DescriptorPoolCreateInfo poolCI {
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1,
        1,
        &poolSize
    };
    pool = vk::raii::DescriptorPool(ctx.getDevice(), poolCI);

    if (last_offset != 0) {
        buf = new vulkan::Buffer(last_offset, vk::BufferUsageFlagBits::eUniformBuffer);

        data = buf->getMappedMemory();
        memset(data, 0, last_offset);

        vk::DescriptorSetAllocateInfo descSetAllocI {*pool, 1, &*setLayout};
        descSet = std::move(ctx.getDevice().allocateDescriptorSets(descSetAllocI)[0]);

        vk::DescriptorBufferInfo bufferDescInfo {*buf->getBuffer(), 0, last_offset};
        vk::WriteDescriptorSet write {*descSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferDescInfo};
        ctx.getDevice().updateDescriptorSets(write, nullptr);
    }
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