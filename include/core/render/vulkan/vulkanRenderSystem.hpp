#pragma once
#include <core/render/vulkan/renderer.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {
    
class VulkanRenderSystem : public RenderSystem {
private:
    VulkanRenderer renderer;
    Uniform uniform;
    std::vector<RenderComponent*> components;

public:
    VulkanRenderSystem() {
        initStages.emplace_back([this]() {});
        initStages.emplace_back([this]() {uniform.initBuffer();});
    }

    void addComponent(RenderComponent* comp) override;
    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;

    Uniform& getUniform() override { return uniform; }

    ~VulkanRenderSystem() override;
};

}