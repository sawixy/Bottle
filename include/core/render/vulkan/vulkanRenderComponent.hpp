#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/renderSystem.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderComponent {
private:
    vk::raii::Pipeline pipeline;
    std::vector<vk::raii::ShaderModule> shaders;
    Mesh mesh;
    vk::raii::Buffer vertices{nullptr};
    vk::raii::Buffer indices{nullptr};

    void loadBuffers();

public:
    VulkanRenderComponent(vk::raii::Pipeline pipeline,std::vector<vk::raii::ShaderModule> shaders) : pipeline(std::move(pipeline)), shaders(std::move(shaders)) {}

    vk::raii::Pipeline& getPipeline() { return pipeline; }
    Mesh getMesh() { return mesh; }
};

}