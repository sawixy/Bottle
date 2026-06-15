#include <core/utils/locator.hpp>
#include "core/render/renderComponent.hpp"
#include "vulkan/vulkan.hpp"
#include <core/render/renderSystem.hpp>

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <core/render/vulkan/vulkanRenderComponent.hpp>
#include <core/config/configSystem.hpp>

namespace bottle::core::render {

RenderComponent::RenderComponent(Mesh mesh, std::vector<resources::render::ShaderResource*> shaders, std::vector<std::unordered_map<std::string, Uniform::UniformType>> uniforms) {
    if (utils::Locator::Instance().get<core::config::ConfigSystem>()->get<std::string>("renderer.api") == "vulkan") {
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

        inner = new vulkan::VulkanRenderComponentInner(mesh, builder, shaders, std::move(uniforms));
        utils::Locator::Instance().get<RenderSystem>()->addComponent(this);
    } else {
        throw std::runtime_error("Unsupported renderer API");
    }
}

}