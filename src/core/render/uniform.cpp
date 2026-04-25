#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "core/utils/locator.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/uniform.hpp>
#include <core/render/vulkan/context.hpp>

namespace bottle::core::render {

vk::DescriptorSetLayoutBinding Uniform::getBinding() {
    return vk::DescriptorSetLayoutBinding {
        0,  // Default binding
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        nullptr
    };
}

void Uniform::initBuffer(vk::raii::DescriptorPool& pool, vk::raii::DescriptorSetLayout& layout) {
    vulkan::Context& ctx = dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();

    
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