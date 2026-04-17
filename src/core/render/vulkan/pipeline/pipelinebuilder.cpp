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

    vertexInputCI.setVertexBindingDescriptionCount(static_cast<uint32_t>(vertexBindings.size()));
    vertexInputCI.setPVertexBindingDescriptions(vertexBindings.data());
    vertexInputCI.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributes.size()));
    vertexInputCI.setPVertexAttributeDescriptions(vertexAttributes.data());

    // TODO: Make uniform, push constants support
    vk::PipelineLayoutCreateInfo layoutCI {
        {},
        0,
        nullptr,
        0,
        nullptr,
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