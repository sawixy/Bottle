#pragma once

#include <core/resources/render/shaderResource.hpp>
#include <vector>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::resources::render::vulkan {

class VulkanShaderResource : public ShaderResource {
private:
    std::vector<uint32_t> code;

public:
    VulkanShaderResource(std::string name, std::string path, ShaderType type)
        : ShaderResource(std::move(name), std::move(path), type) {}

    void load() override; /* path must be compiled shader */
    void unload() override;
    ShaderCode getCode() override;
    bool empty() override { return code.empty(); }
};

}