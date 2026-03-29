#pragma once

#include <core/resources/render/shaderResource.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::resources::render::vulkan {

class VulkanShaderResource : public ShaderResource {
private:
    std::vector<uint32_t> code;
    vk::raii::ShaderModule module{nullptr};

public:
    void load(std::string path) override; /* path must be compiled shader */
    void unload() override;
    ShaderCode getCode() override;
    bool empty() override { return code.empty() ? true : false; }
};

}