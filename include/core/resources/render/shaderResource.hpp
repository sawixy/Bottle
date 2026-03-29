#pragma once

#include <string>
#include <variant>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::resources::render {

struct ShaderCode {
    std::variant<vk::raii::ShaderModule, std::string> code;

    ShaderCode();
};

class ShaderResource {
protected:
    bool isLoaded;
    std::string path;

public:
    virtual void load(std::string path) = 0;
    virtual void unload() = 0;
    virtual bool loaded() { return isLoaded; }
    virtual bool empty() = 0;

    virtual ShaderCode getCode() = 0;
    const std::string getPath() const { return path; }
};

}