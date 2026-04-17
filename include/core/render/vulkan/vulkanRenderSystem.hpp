#pragma once
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {
    
class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;
    std::vector<RenderComponent*> components;

public:
    VulkanRenderSystem() {}

    void addComponent(RenderComponent* comp) override;
    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;

    ~VulkanRenderSystem() override;
};

}