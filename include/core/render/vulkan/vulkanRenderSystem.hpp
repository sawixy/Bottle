#pragma once
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;

public:
    VulkanRenderSystem() {}

    void update() override;

    ~VulkanRenderSystem() override;
};

}