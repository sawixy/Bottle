#include <core/utils/locator.hpp>
#include "core/render/renderComponent.hpp"
#include <core/render/renderSystem.hpp>

#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <core/render/vulkan/vulkanRenderComponent.hpp>
namespace bottle::core::render {

RenderComponent::RenderComponent(Mesh mesh, std::vector<resources::render::ShaderResource*> shaders) {
    vulkan::PipelineBuilder builder;
    for (resources::render::ShaderResource* shader : shaders) {
        builder.addShader(shader);
    }
    builder
        .addVertexBinding(0, sizeof(Vertex), vk::VertexInputRate::eVertex)
        .addVertexAttribute(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, x))
        .addVertexAttribute(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, r))
        .addColorFormat(dynamic_cast<vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getFormat())
        .setDepthFormat(vk::Format::eD32Sfloat);
    builder.build();


    inner = new vulkan::VulkanRenderComponentInner(std::move(builder.getPipeline()), mesh, builder.getModules(), shaders);
    utils::Locator::Instance().get<RenderSystem>()->addComponent(this);
}

}