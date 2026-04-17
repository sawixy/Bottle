#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <core/resources/render/vulkan/VulkanShaderResource.hpp>
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <memory>

namespace bottle::core::render::vulkan {

static bool initVulkanDispatcher = []() {
    vk::detail::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    return true;
}();

VulkanRenderSystem::~VulkanRenderSystem() {};

void VulkanRenderSystem::addComponent(RenderComponent* comp) {
    components.push_back(comp);
}

void VulkanRenderSystem::update() {
    renderer.render(components);
}

}