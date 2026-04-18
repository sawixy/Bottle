#pragma once

#include "vulkanRenderSystem.hpp"
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <core/render/renderComponent.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderComponentInner : public RenderComponentInner {
private:
    vk::raii::Pipeline pipeline;
    vk::raii::Buffer vertices{nullptr};
    vk::raii::DeviceMemory verticesMemory{nullptr};
    vk::raii::Buffer indices{nullptr};
    vk::raii::DeviceMemory indicesMemory{nullptr};
    std::vector<vk::raii::ShaderModule> shaderModules;
    std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponentInner(vk::raii::Pipeline pipeline, Mesh mesh, std::vector<vk::raii::ShaderModule> shaders, std::vector<resources::render::ShaderResource*> shaderResources, std::vector<vk::raii::DescriptorSetLayout> sets)
        : RenderComponentInner(mesh, shaderResources), pipeline(std::move(pipeline)), shaderModules(std::move(shaders)), descriptorSetLayouts(std::move(sets)) {
        initBuffers();
        loadBuffers();
    }

    void setMesh(Mesh newMesh) override {
        mesh = std::move(newMesh);
        loadBuffers();
    }
    vk::raii::Pipeline& getPipeline() { return pipeline; }
    vk::raii::Buffer& getVertexBuffer() { return vertices; }
    vk::raii::Buffer& getIndexBuffer() { return indices; }
};

}