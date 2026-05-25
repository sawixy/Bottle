#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/renderComponent.hpp>
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <core/render/uniform.hpp>

#ifdef DEBUG
#include <iostream>
#endif

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

    Uniform uniform;

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponentInner(Mesh mesh, PipelineBuilder& builder, std::vector<resources::render::ShaderResource*> shaderResources, std::unordered_map<std::string, Uniform::UniformType> uniforms = {})
        : RenderComponentInner(mesh, shaderResources), uniform(std::move(uniforms)) {
        uniform.initBuffer();
        builder.addDescriptorSetLayoutBinding(uniform.getBinding());
        builder.addDescriptorSetLayout(std::move(uniform.getSetLayout()));
        builder.build();
        pipeline = std::move(builder.getPipeline());
        shaderModules = std::move(builder.getModules());
        layout = std::move(builder.getLayout());
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

    Uniform& getUniform() override { return uniform; }

    ~VulkanRenderComponentInner() {
#ifdef DEBUG
        std::cout << "Destroying VulkanRenderComponentInner" << std::endl;
#endif
    }
};

}