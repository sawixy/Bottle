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
    vk::raii::DeviceMemory verticesMemory{nullptr};
    vk::raii::Buffer indices{nullptr};
    vk::raii::DeviceMemory indicesMemory{nullptr};

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponent(vk::raii::Pipeline pipeline,std::vector<vk::raii::ShaderModule> shaders, Mesh mesh) : pipeline(std::move(pipeline)), shaders(std::move(shaders)), mesh(mesh) {}

    vk::raii::Pipeline& getPipeline() { return pipeline; }
    Mesh getMesh() { return mesh; }
    vk::raii::Buffer& getVertexBuffer() { return vertices; }
    vk::raii::Buffer& getIndexBuffer() { return indices; }
};

}