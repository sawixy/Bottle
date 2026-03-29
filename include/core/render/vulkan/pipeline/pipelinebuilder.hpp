#pragma once

#include "vulkan/vulkan.hpp"
#include <array>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <core/resources/render/shaderResource.hpp>
#include <vulkan/vulkan_raii.hpp>
namespace bottle::core::render::vulkan {

struct PipelineWithShaders {
    vk::Pipeline pipeline;
    std::vector<vk::raii::ShaderModule> shaders;
};

class PipelineBuilder {
private:
    std::vector<resources::render::ShaderResource*> shaders; // ShaderResource must be alive while creating Pipeline
    vk::PipelineShaderStageCreateInfo pipelineShaderCI;
    vk::PipelineVertexInputStateCreateInfo vertexInputCI;
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCI;
    std::vector<vk::DynamicState> dynamicStates {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    }; // Viewport and Scissor always dynamic
    vk::PipelineDynamicStateCreateInfo dynamicCI;
    vk::PipelineDepthStencilStateCreateInfo depthStencilCI;
    vk::PipelineRasterizationStateCreateInfo rasterizationCI;
    vk::PipelineMultisampleStateCreateInfo multisamplingCI;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments {
    {
            vk::True,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd
        }
    };
    vk::PipelineColorBlendStateCreateInfo colorBlendCI;

public:
    PipelineBuilder() {
        // defaults
        vertexInputCI.setPVertexAttributeDescriptions(nullptr);
        vertexInputCI.setVertexAttributeDescriptions(nullptr);
        vertexInputCI.setPVertexBindingDescriptions(nullptr);
        vertexInputCI.setVertexBindingDescriptions(nullptr);

        inputAssemblyCI.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssemblyCI.setPrimitiveRestartEnable(vk::False);
        
        depthStencilCI.setDepthTestEnable(vk::False);
        depthStencilCI.setDepthBoundsTestEnable(vk::False);
        depthStencilCI.setStencilTestEnable(vk::False);
        depthStencilCI.setDepthWriteEnable(vk::True);
        depthStencilCI.setMinDepthBounds(0.0);
        depthStencilCI.setMaxDepthBounds(1.0);

        rasterizationCI.setDepthBiasEnable(vk::True);
        rasterizationCI.setDepthClampEnable(vk::False);
        rasterizationCI.setRasterizerDiscardEnable(vk::False);
        rasterizationCI.setPolygonMode(vk::PolygonMode::eFill);
        rasterizationCI.setLineWidth(1.0);
        rasterizationCI.setCullMode(vk::CullModeFlagBits::eBack);
        rasterizationCI.setFrontFace(vk::FrontFace::eClockwise);
        rasterizationCI.setDepthBiasConstantFactor(0.0);
        rasterizationCI.setDepthBiasSlopeFactor(0.0);
        rasterizationCI.setDepthBiasClamp(0.0);

        multisamplingCI.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisamplingCI.setSampleShadingEnable(vk::True);
        multisamplingCI.setAlphaToCoverageEnable(vk::False);
        multisamplingCI.setPSampleMask(nullptr);
        multisamplingCI.setAlphaToOneEnable(vk::False);
        multisamplingCI.setMinSampleShading(1.0);

        colorBlendCI.setAttachmentCount(attachments.size());
        colorBlendCI.setPAttachments(attachments.data());
        colorBlendCI.setBlendConstants(std::array<float, 4>{ 0.0, 0.0, 0.0, 0.0 });
        colorBlendCI.setLogicOpEnable(vk::False);
        colorBlendCI.setLogicOp(vk::LogicOp::eCopy);
    }

    PipelineBuilder& addShader(resources::render::ShaderResource* shader) { shaders.push_back(shader); return *this; }
    PipelineBuilder& addDynamicState(vk::DynamicState dynState) { dynamicStates.push_back(dynState); return *this; }
    PipelineBuilder& setTopology(vk::PrimitiveTopology topology) { inputAssemblyCI.setTopology(topology); return *this; }
    PipelineBuilder& setPolygonMode(vk::PolygonMode polygonMode) { rasterizationCI.setPolygonMode(polygonMode); return *this; }
    PipelineBuilder& setSampleCount(vk::SampleCountFlagBits sampleCount) { multisamplingCI.setRasterizationSamples(sampleCount); return *this; }
    PipelineBuilder& addColorBlendAttachment(vk::PipelineColorBlendAttachmentState attachment) { attachments.push_back(attachment); colorBlendCI.setAttachmentCount(attachments.size()); return *this; }
    PipelineWithShaders build();
};

}