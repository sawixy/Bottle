#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>
#include <core/resources/render/vulkan/VulkanShaderResource.hpp>
#include <core/utils/locator.hpp>

#include <fstream>
#include <ios>

namespace bottle::core::resources::render::vulkan {
    
std::vector<char> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::ate | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0);
    std::vector<char> buf(size);
    in.read(buf.data(), size);
    in.close();
    return buf;
}

void VulkanShaderResource::load() {
    auto bytes = readFile(path);
    size_t uintCount = bytes.size() / sizeof(uint32_t);
    code.resize(uintCount);
    std::memcpy(code.data(), bytes.data(), uintCount * sizeof(uint32_t));
    isLoaded = !code.empty();
}

void VulkanShaderResource::unload() {
    code.clear();
    isLoaded = false;
}

ShaderCode VulkanShaderResource::getCode() {
    if (!loaded()) {
        throw std::runtime_error("Shader resource not loaded: " + getPath());
    }

    const vk::raii::Device& dev = dynamic_cast<core::render::vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<core::render::RenderSystem>())->getRenderer().getContext().getDevice();

    vk::ShaderModuleCreateInfo shaderModuleCI {
        {},
        code.size(),
        code.data(),
        nullptr
    };

    vk::raii::ShaderModule module {
        dev,
        shaderModuleCI,
        nullptr
    };

    return ShaderCode{std::move(module)};
}

}