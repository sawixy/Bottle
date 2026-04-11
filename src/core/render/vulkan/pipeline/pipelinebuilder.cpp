#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/context.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "core/resources/render/shaderResource.hpp"
#include "core/utils/locator.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {

vk::raii::Pipeline PipelineBuilder::build() {
    std::vector<vk::PipelineShaderStageCreateInfo> shadersCI;

    for (auto shader : shaders) {
        // TODO: Make exception checking
        shadersCI.push_back(vk::PipelineShaderStageCreateInfo {
            {},
            (shader->getType() == resources::render::ShaderResource::ShaderType::VERTEX) ? vk::ShaderStageFlagBits::eVertex : (shader->getType() == resources::render::ShaderResource::ShaderType::FRAGMENT ? vk::ShaderStageFlagBits::eFragment : vk::ShaderStageFlagBits::eCompute),
            std::get<vk::raii::ShaderModule>(shader->getCode().code),
            shader->getName().c_str(),
            nullptr,
            nullptr
        });
    }

    // TODO: Make uniform, push constants support
    vk::PipelineLayoutCreateInfo layoutCI {
        {},
        1,
        nullptr,
        1,
        nullptr,
        nullptr
    };
    vk::PipelineLayout layout = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext().getDevice().createPipelineLayout(layoutCI);

    // TODO: Make inherit pipelines
    vk::GraphicsPipelineCreateInfo pipelineCI {
        {},
        static_cast<uint32_t>(shaders.size()),
        shadersCI.data(),
        &vertexInputCI,
        &inputAssemblyCI,
        nullptr,
        nullptr,
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
        nullptr
    };

    return std::move(dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getContext().getDevice().createGraphicsPipeline(nullptr, pipelineCI));
}

}