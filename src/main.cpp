#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include <core/utils/ecs/entity.hpp>
#include <core/utils/locator.hpp>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <memory>
#include <vector>

int main() {
    bottle::utils::Locator::Instance().add<bottle::core::window::WindowSystem>(new bottle::core::window::glfw::GLFWWindowSystem{});
    bottle::utils::Locator::Instance().add<bottle::core::render::RenderSystem>(new bottle::core::render::vulkan::VulkanRenderSystem{});

    auto* renderSystem = bottle::utils::Locator::Instance().get<bottle::core::render::RenderSystem>();
    if (!renderSystem) {
        return -1;
    }

    auto vertShader = renderSystem->createShaderResource(
        "main", "shaders/triangle.vert.spv",
        bottle::core::resources::render::ShaderResource::ShaderType::VERTEX);
    auto fragShader = renderSystem->createShaderResource(
        "main", "shaders/triangle.frag.spv",
        bottle::core::resources::render::ShaderResource::ShaderType::FRAGMENT);

    vertShader->load();
    fragShader->load();

    std::vector<bottle::core::resources::render::ShaderResource*> shaderList{
        vertShader.get(),
        fragShader.get()
    };

    bottle::core::render::Mesh mesh;
    mesh.vertices = {
        {0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
        {-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f}
    };
    mesh.indices = {0, 1, 2};
    std::cout << "Mesh created: vertices=" << mesh.vertices.size() << ", indices=" << mesh.indices.size() << std::endl;

    auto component = renderSystem->createComponent(std::move(mesh), shaderList);
    auto* componentPtr = component.get();

    bottle::utils::Entity entity;
    entity.addComponent(std::make_any<bottle::core::render::RenderComponent*>(componentPtr));
    bottle::utils::Locator::Instance().add("test", entity);

    renderSystem->addComponent(std::move(component));

    auto w = bottle::utils::Locator::Instance().get<bottle::core::window::WindowSystem>();

    while (1) {
        renderSystem->update();
        w->update();
    }
    
    return 0;
}