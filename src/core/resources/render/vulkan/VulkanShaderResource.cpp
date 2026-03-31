#include "core/render/renderSystem.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>
#include <core/resources/render/vulkan/VulkanShaderResource.hpp>
#include <core/utils/locator.hpp>

#include <fstream>
#include <ios>

namespace bottle::core::resources::render::vulkan {
    
std::vector<char> readFile(std::string path) {
    std::ifstream in(path, std::ios::ate);
    size_t size = in.tellg();
    in.seekg(std::ios::beg);
    std::vector<char> buf(size);
    in.read(buf.data(), size);
    in.close();
    return buf;
}

// TODO: make autocompile shaders
void VulkanShaderResource::load(std::string path) {
    this->path = path;
    std::vector<char> code = readFile(path);
    isLoaded = true;
}

void VulkanShaderResource::unload() {
    module.clear();
    code.clear();
    isLoaded = false;
}

ShaderCode VulkanShaderResource::getCode() {
    const vk::raii::Device& dev = dynamic_cast<core::render::vulkan::VulkanRenderSystem*>(utils::Locator::Instance().get<core::render::RenderSystem>())->getRenderer().getContext().getDevice();

    vk::ShaderModuleCreateInfo shaderModuleCI {
        {},
        code.size(),
        reinterpret_cast<uint32_t*>(code.data()),
        nullptr
    };

    vk::raii::ShaderModule mod {
        dev,
        shaderModuleCI,
        nullptr
    };

    ShaderCode code {std::move(mod)};

    return std::move(code);
}

}