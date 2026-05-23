#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/renderComponent.hpp>

#include <iostream>

namespace bottle::core::render::vulkan {

class VulkanRenderComponentInner : public RenderComponentInner {
private:
    vk::raii::Pipeline pipeline{nullptr};
    vk::raii::Buffer vertices{nullptr};
    vk::raii::DeviceMemory verticesMemory{nullptr};
    vk::raii::Buffer indices{nullptr};
    vk::raii::DeviceMemory indicesMemory{nullptr};
    std::vector<vk::raii::ShaderModule> shaderModules;
    vk::raii::PipelineLayout layout{nullptr};

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponentInner(vk::raii::Pipeline pipeline, Mesh mesh, std::vector<vk::raii::ShaderModule> shaders, std::vector<resources::render::ShaderResource*> shaderResources, vk::raii::PipelineLayout layout)
        : RenderComponentInner(mesh, shaderResources), pipeline(std::move(pipeline)), shaderModules(std::move(shaders)), layout(std::move(layout)) {
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
    vk::raii::PipelineLayout& getLayout() { return layout; }

    ~VulkanRenderComponentInner() {
        std::cout << "Destroying VulkanRenderComponentInner" << std::endl;
    }
};

}