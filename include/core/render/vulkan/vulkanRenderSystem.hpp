#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {
    
class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;
    std::vector<std::unique_ptr<VulkanRenderComponent>> components;

public:
    VulkanRenderSystem() {}

    void addComponent(std::unique_ptr<RenderComponent> comp) override;
    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;

    ~VulkanRenderSystem() override;
};

}