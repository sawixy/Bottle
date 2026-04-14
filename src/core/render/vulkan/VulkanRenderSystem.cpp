#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/resources/render/vulkan/VulkanShaderResource.hpp>
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <memory>
#include <stdexcept>
#include <cstddef>

namespace bottle::core::render::vulkan {

static bool initVulkanDispatcher = []() {
    vk::detail::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    return true;
}();

VulkanRenderSystem::~VulkanRenderSystem() {};

void VulkanRenderSystem::addComponent(std::unique_ptr<RenderComponent> comp) {
    auto vkComp = dynamic_cast<VulkanRenderComponent*>(comp.get());
    if (!vkComp) {
        throw std::runtime_error("VulkanRenderSystem::addComponent requires VulkanRenderComponent");
    }

    components.emplace_back(static_cast<VulkanRenderComponent*>(comp.release()));
}

std::unique_ptr<resources::render::ShaderResource> VulkanRenderSystem::createShaderResource(
    const std::string& name,
    const std::string& path,
    resources::render::ShaderResource::ShaderType type) {
    return std::make_unique<bottle::core::resources::render::vulkan::VulkanShaderResource>(name, path, type);
}

std::unique_ptr<RenderComponent> VulkanRenderSystem::createComponent(
    Mesh mesh,
    std::vector<resources::render::ShaderResource*> shaders) {
    PipelineBuilder builder;
    for (auto shader : shaders) {
        builder.addShader(shader);
    }

    builder
        .addVertexBinding(0, sizeof(Vertex), vk::VertexInputRate::eVertex)
        .addVertexAttribute(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, x))
        .addVertexAttribute(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, r))
        .addColorFormat(renderer.getFormat())
        .setDepthFormat(vk::Format::eD32Sfloat);

    builder.build();

    auto pipeline = std::move(builder.getPipeline());
    return std::make_unique<VulkanRenderComponent>(std::move(pipeline), std::move(mesh), std::move(shaders), std::move(builder.getModules()));
}

void VulkanRenderSystem::update() {
    std::vector<VulkanRenderComponent*> rawComponents;
    rawComponents.reserve(components.size());

    for (auto& comp : components) {
        rawComponents.push_back(comp.get());
    }

    renderer.render(rawComponents);
}

}