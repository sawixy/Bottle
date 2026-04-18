#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
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
    vk::raii::DescriptorSetLayout descriptorSetLayout;
    vk::raii::DescriptorPool pool;
    vk::raii::PipelineLayout layout{nullptr};

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponentInner(vk::raii::Pipeline pipeline, Mesh mesh, std::vector<vk::raii::ShaderModule> shaders, std::vector<resources::render::ShaderResource*> shaderResources, vk::raii::DescriptorSetLayout sets, vk::raii::DescriptorPool pool, vk::raii::PipelineLayout layout)
        : RenderComponentInner(mesh, shaderResources), pipeline(std::move(pipeline)), shaderModules(std::move(shaders)), descriptorSetLayout(std::move(sets)), pool(std::move(pool)), layout(std::move(layout)) {
        initBuffers();
        loadBuffers();
    }

    void setUniforms();
    void setMesh(Mesh newMesh) override {
        mesh = std::move(newMesh);
        loadBuffers();
    }
    vk::raii::Pipeline& getPipeline() { return pipeline; }
    vk::raii::Buffer& getVertexBuffer() { return vertices; }
    vk::raii::Buffer& getIndexBuffer() { return indices; }
};

}