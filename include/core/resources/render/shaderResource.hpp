#pragma once

#include <string>
#include <variant>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::resources::render {

struct ShaderCode {
    std::variant<vk::raii::ShaderModule, std::string> code;

    ShaderCode(vk::raii::ShaderModule module) : code(std::move(module)) {}
    ShaderCode(std::string code) : code(code) {}
};

class ShaderResource {
public:
    enum class ShaderType {
        COMPUTE,
        VERTEX,
        FRAGMENT
    };

protected:
    bool isLoaded = false;
    std::string path;
    ShaderType type = ShaderType::VERTEX;
    std::string name;

public:
    ShaderResource(std::string name, std::string path, ShaderType type) : path(std::move(path)), type(type), name(std::move(name)) {
    }

    virtual void load() = 0;
    virtual void unload() = 0;
    virtual bool loaded() const { return isLoaded; }
    virtual bool empty() = 0;

    virtual ShaderCode getCode() = 0;
    const std::string& getPath() const { return path; }
    ShaderType getType() const { return type; }
    const std::string& getName() const { return name; }
};

}