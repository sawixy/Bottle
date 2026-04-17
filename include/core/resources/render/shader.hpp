#pragma once

#include <core/resources/render/shaderResource.hpp>
#include <core/resources/render/vulkan/VulkanShaderResource.hpp>
namespace bottle::core::resources::render {

class Shader {
public:
    enum class TYPE {
        VERTEX,
        FRAGMENT,
        COMPUTE
    };
private:
    ShaderResource* resource;

public:
    Shader(std::string path, Shader::TYPE type) {
        resource = new vulkan::VulkanShaderResource("main", path, (type == Shader::TYPE::FRAGMENT ? vulkan::VulkanShaderResource::ShaderType::FRAGMENT : vulkan::VulkanShaderResource::ShaderType::VERTEX));
        resource->load();
    }

    ShaderResource* getResource() { return resource; }
};

}