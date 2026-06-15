#pragma once
#include "core/render/uniform.hpp"
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {
    
class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;
    std::vector<RenderComponent*> components;
    std::vector<Uniform> uniforms;

public:
    VulkanRenderSystem() {
        initStages.emplace_back([this]() { });
    }

    void addComponent(RenderComponent* comp) override;
    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;
    Uniform& getUniform(int binding) override { return uniforms[binding]; }

    ~VulkanRenderSystem() override;
};

}