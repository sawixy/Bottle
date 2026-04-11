#include <core/render/vulkan/vulkanRenderSystem.hpp>
#include <stdexcept>

#include <iostream>

namespace bottle::core::render::vulkan {

VulkanRenderSystem::~VulkanRenderSystem() {};

void VulkanRenderSystem::addComponent(std::unique_ptr<RenderComponent> comp) {
    auto vkComp = dynamic_cast<VulkanRenderComponent*>(comp.get());
    if (!vkComp) {
        throw std::runtime_error("VulkanRenderSystem::addComponent requires VulkanRenderComponent");
    }

    components.emplace_back(static_cast<VulkanRenderComponent*>(comp.release()));
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