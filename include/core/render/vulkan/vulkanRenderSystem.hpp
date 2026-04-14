#pragma once
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
    std::unique_ptr<resources::render::ShaderResource> createShaderResource(
        const std::string& name,
        const std::string& path,
        resources::render::ShaderResource::ShaderType type) override;
    std::unique_ptr<RenderComponent> createComponent(
        Mesh mesh,
        std::vector<resources::render::ShaderResource*> shaders) override;
    VulkanRenderer& getRenderer() { return renderer; };
    void update() override;

    ~VulkanRenderSystem() override;
};

}