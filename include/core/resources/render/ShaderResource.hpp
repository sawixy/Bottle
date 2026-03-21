#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <variant>

namespace bottle::core::resources::render {

using VulkanCode = std::vector<uint32_t>;
using OpenGLCode = std::string;

struct ShaderCode {
    std::variant<VulkanCode, OpenGLCode> code;
};

class ShaderResource {
protected:
    std::string path;

public:
    virtual void load(std::string path) = 0;
    virtual void unload() = 0;
    virtual bool loaded() = 0;
    virtual bool empty() = 0;

    virtual ShaderCode getCode() const = 0;
    const std::string getPath() const { return path; }
};

}