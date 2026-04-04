#pragma once
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {
    
class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;

public:
    VulkanRenderSystem() {}

    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;

    ~VulkanRenderSystem() override;
};

}