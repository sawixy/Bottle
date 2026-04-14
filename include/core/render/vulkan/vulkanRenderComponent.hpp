#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/renderSystem.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderComponent : public RenderComponent {
private:
    vk::raii::Pipeline pipeline;
    vk::raii::Buffer vertices{nullptr};
    vk::raii::DeviceMemory verticesMemory{nullptr};
    vk::raii::Buffer indices{nullptr};
    vk::raii::DeviceMemory indicesMemory{nullptr};
    std::vector<vk::raii::ShaderModule> shaderModules;

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponent(vk::raii::Pipeline pipeline, Mesh mesh, std::vector<resources::render::ShaderResource*> shaders, std::vector<vk::raii::ShaderModule> shaderModules)
        : RenderComponent(std::move(mesh), std::move(shaders)), pipeline(std::move(pipeline)), shaderModules(std::move(shaderModules)) {
        initBuffers();
        loadBuffers();
    }

    vk::raii::Pipeline& getPipeline() { return pipeline; }
    const Mesh& getMesh() const { return RenderComponent::getMesh(); }
    Mesh& getMesh() { return RenderComponent::getMesh(); }
    const std::vector<resources::render::ShaderResource*>& getShaders() const { return RenderComponent::getShaders(); }
    std::vector<resources::render::ShaderResource*>& getShaders() { return RenderComponent::getShaders(); }
    vk::raii::Buffer& getVertexBuffer() { return vertices; }
    vk::raii::Buffer& getIndexBuffer() { return indices; }
};

}