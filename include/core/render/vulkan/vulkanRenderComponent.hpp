#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/renderComponent.hpp>
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <core/render/uniform.hpp>
#include <core/render/vulkan/buffer.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace bottle::core::render::vulkan {

class VulkanRenderComponentInner : public RenderComponentInner {
private:
    vk::raii::Pipeline pipeline{nullptr};
    Buffer* vertices;
    Buffer* indices;
    std::vector<vk::raii::ShaderModule> shaderModules;
    vk::raii::PipelineLayout layout{nullptr};

    std::vector<Uniform> uniforms;

    void loadBuffers();
    void initBuffers();

public:
    VulkanRenderComponentInner(Mesh mesh, PipelineBuilder& builder, std::vector<resources::render::ShaderResource*> shaderResources, std::vector<std::unordered_map<std::string, Uniform::UniformType>> uniforms = {})
        : RenderComponentInner(mesh, shaderResources) {
        for (auto uniform_type : uniforms) {
            this->uniforms.emplace_back(uniform_type);
            this->uniforms.back().initBuffer();
            builder.addDescriptorSetLayoutBinding(this->uniforms.back().getBinding());
            builder.addDescriptorSetLayout(std::move(this->uniforms.back().getSetLayout()));
        }
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
    vk::raii::Buffer& getVertexBuffer() { return vertices->getBuffer(); }
    vk::raii::Buffer& getIndexBuffer() { return indices->getBuffer(); }
    vk::raii::PipelineLayout& getLayout() { return layout; }

    Uniform& getUniform(int binding) override { return uniforms[binding]; }

    ~VulkanRenderComponentInner() {
#ifdef DEBUG
        std::cout << "Destroying VulkanRenderComponentInner" << std::endl;
#endif
    }
};

}