#pragma once

#include "vulkan/vulkan.hpp"
#include <array>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <core/resources/render/shaderResource.hpp>
#include <vulkan/vulkan_raii.hpp>
namespace bottle::core::render::vulkan {

class PipelineBuilder {
private:
    vk::raii::Pipeline pipeline{nullptr};
    std::vector<vk::raii::ShaderModule> modules;
    vk::raii::PipelineLayout layout{nullptr};

    std::vector<resources::render::ShaderResource*> shaders; // ShaderResource must be alive while creating Pipeline
    vk::PipelineShaderStageCreateInfo pipelineShaderCI;
    vk::PipelineVertexInputStateCreateInfo vertexInputCI;
    std::vector<vk::VertexInputBindingDescription> vertexBindings;
    std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCI;
    std::vector<vk::DynamicState> dynamicStates {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    }; // Viewport and Scissor always dynamic
    vk::PipelineDynamicStateCreateInfo dynamicCI;
    vk::PipelineDepthStencilStateCreateInfo depthStencilCI;
    vk::PipelineRasterizationStateCreateInfo rasterizationCI;
    vk::PipelineMultisampleStateCreateInfo multisamplingCI;
    vk::PipelineViewportStateCreateInfo viewportCI;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments {
    {
            vk::True,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
        }
    };
    vk::PipelineColorBlendStateCreateInfo colorBlendCI;
    std::vector<vk::Format> colorFormats;
    vk::Format depthFormat = vk::Format::eUndefined;

public:
    PipelineBuilder() {
        // defaults
        vertexInputCI.setVertexAttributeDescriptionCount(0);
        vertexInputCI.setPVertexAttributeDescriptions(nullptr);
        vertexInputCI.setVertexBindingDescriptionCount(0);
        vertexInputCI.setPVertexBindingDescriptions(nullptr);

        inputAssemblyCI.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssemblyCI.setPrimitiveRestartEnable(vk::False);
        
        depthStencilCI.setDepthTestEnable(vk::False);
        depthStencilCI.setDepthBoundsTestEnable(vk::False);
        depthStencilCI.setStencilTestEnable(vk::False);
        depthStencilCI.setDepthWriteEnable(vk::True);
        depthStencilCI.setMinDepthBounds(0.0);
        depthStencilCI.setMaxDepthBounds(1.0);

        rasterizationCI.setDepthBiasEnable(vk::False);
        rasterizationCI.setDepthClampEnable(vk::False);
        rasterizationCI.setRasterizerDiscardEnable(vk::False);
        rasterizationCI.setPolygonMode(vk::PolygonMode::eFill);
        rasterizationCI.setLineWidth(1.0);
        rasterizationCI.setCullMode(vk::CullModeFlagBits::eNone);
        rasterizationCI.setFrontFace(vk::FrontFace::eCounterClockwise);
        rasterizationCI.setDepthBiasConstantFactor(0.0);
        rasterizationCI.setDepthBiasSlopeFactor(0.0);
        rasterizationCI.setDepthBiasClamp(0.0);
        
        viewportCI.setViewportCount(1);
        viewportCI.setPViewports(nullptr);
        viewportCI.setScissorCount(1);
        viewportCI.setPScissors(nullptr);

        multisamplingCI.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisamplingCI.setSampleShadingEnable(vk::True);
        multisamplingCI.setAlphaToCoverageEnable(vk::False);
        multisamplingCI.setPSampleMask(nullptr);
        multisamplingCI.setAlphaToOneEnable(vk::False);
        multisamplingCI.setMinSampleShading(1.0);

        dynamicCI.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicCI.setPDynamicStates(dynamicStates.data());

        colorBlendCI.setAttachmentCount(attachments.size());
        colorBlendCI.setPAttachments(attachments.data());
        colorBlendCI.setBlendConstants(std::array<float, 4>{ 0.0, 0.0, 0.0, 0.0 });
        colorBlendCI.setLogicOpEnable(vk::False);
        colorBlendCI.setLogicOp(vk::LogicOp::eCopy);
    }

    PipelineBuilder& addShader(resources::render::ShaderResource* shader) { shaders.push_back(shader); return *this; }
    PipelineBuilder& addVertexBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex) {
        vertexBindings.push_back(vk::VertexInputBindingDescription{binding, stride, inputRate});
        return *this;
    }
    PipelineBuilder& addVertexAttribute(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset) {
        vertexAttributes.push_back(vk::VertexInputAttributeDescription{location, binding, format, offset});
        return *this;
    }
    PipelineBuilder& setDepthFormat(vk::Format format) { depthFormat = format; return *this; }
    PipelineBuilder& addColorFormat(vk::Format format) { colorFormats.push_back(format); return *this; }
    PipelineBuilder& addDynamicState(vk::DynamicState dynState) { dynamicStates.push_back(dynState); return *this; }
    PipelineBuilder& setTopology(vk::PrimitiveTopology topology) { inputAssemblyCI.setTopology(topology); return *this; }
    PipelineBuilder& setPolygonMode(vk::PolygonMode polygonMode) { rasterizationCI.setPolygonMode(polygonMode); return *this; }
    PipelineBuilder& setSampleCount(vk::SampleCountFlagBits sampleCount) { multisamplingCI.setRasterizationSamples(sampleCount); return *this; }
    PipelineBuilder& addColorBlendAttachment(vk::PipelineColorBlendAttachmentState attachment) { attachments.push_back(attachment); colorBlendCI.setAttachmentCount(attachments.size()); return *this; }
    void build();
    vk::raii::Pipeline getPipeline() { return std::move(pipeline); }
    std::vector<vk::raii::ShaderModule> getModules() { return std::move(modules); }
    vk::raii::PipelineLayout getLayout() { return std::move(layout); }
};

}