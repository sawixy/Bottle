#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/context.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "core/resources/render/shaderResource.hpp"
#include "core/utils/locator.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <iostream>

namespace bottle::core::render::vulkan {

PipelineBuilder::PipelineBuilder() {
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

void PipelineBuilder::build() {
    std::vector<vk::PipelineShaderStageCreateInfo> shadersCI;
    modules.clear();

    for (auto shader : shaders) {
        modules.push_back(std::move(std::get<vk::raii::ShaderModule>(shader->getCode().code)));

        // TODO: Make exception checking
        shadersCI.push_back(vk::PipelineShaderStageCreateInfo {
            {},
            (shader->getType() == resources::render::ShaderResource::ShaderType::VERTEX) ? vk::ShaderStageFlagBits::eVertex : (shader->getType() == resources::render::ShaderResource::ShaderType::FRAGMENT ? vk::ShaderStageFlagBits::eFragment : vk::ShaderStageFlagBits::eCompute),
            *modules.back(),
            shader->getName().c_str(),
            nullptr,
            nullptr
        });
    }

    auto &ctx = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext();

    vertexInputCI.setVertexBindingDescriptionCount(static_cast<uint32_t>(vertexBindings.size()));
    vertexInputCI.setPVertexBindingDescriptions(vertexBindings.data());
    vertexInputCI.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributes.size()));
    vertexInputCI.setPVertexAttributeDescriptions(vertexAttributes.data());

    vk::PipelineLayoutCreateInfo layoutCI {
        {},
        0,
        nullptr,
        0,
        nullptr
    };
    layout = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext().getDevice().createPipelineLayout(layoutCI);

    vk::PipelineRenderingCreateInfo renderingCI {
        {},
        static_cast<uint32_t>(colorFormats.size()),
        colorFormats.data(),
        depthFormat,
        vk::Format::eUndefined,
        nullptr
    };

    std::cout << "Building pipeline with " << shadersCI.size() << " shader stages, " << vertexBindings.size() << " vertex bindings, " << vertexAttributes.size() << " vertex attributes, " << attachments.size() << " color blend attachments, " << dynamicStates.size() << " dynamic states." << std::endl;

    // TODO: Make inherit pipelines
    vk::GraphicsPipelineCreateInfo pipelineCI {
        {},
        static_cast<uint32_t>(shadersCI.size()),
        shadersCI.data(),
        &vertexInputCI,
        &inputAssemblyCI,
        nullptr,
        &viewportCI,
        &rasterizationCI,
        &multisamplingCI,
        &depthStencilCI,
        &colorBlendCI,
        &dynamicCI,
        layout,
        nullptr,
        0,
        nullptr,
        0,
        &renderingCI
    };

    pipeline = std::move(dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext().getDevice().createGraphicsPipeline(nullptr, pipelineCI));
}

}