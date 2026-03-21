#pragma once

#include <core/resources/render/ShaderResource.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace bottle::core::resources::render::vulkan {

class VulkanShaderResource : public ShaderResource {
private:
    std::string path;
    std::vector<uint32_t> code;

public:
    void load(std::string path) override;
    std::vector<uint32_t> getCode() { return code; }

    bool empty() override { return code.empty() ? true : false; }
};

}