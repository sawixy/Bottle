#pragma once

#include "vulkan/vulkan.hpp"
#include <string>
#include <unordered_map>
namespace bottle::core::render::vulkan {

class Uniform {
public:
    enum class UniformType {
        FLOAT,
        INT,
        UINT,
        VEC2,
        VEC3,
        MAT3,
        MAT4,
        TEXTURE
    };

private:
    void* data;

    std::unordered_map<std::string, UniformType> map;

    size_t getSize(UniformType type) {
        switch (type) {
        case UniformType::UINT: return 4;
        case UniformType::INT: return 4;
        case UniformType::FLOAT: return 4;
        case UniformType::MAT3: return 48;
        case UniformType::MAT4: return 64;
        case UniformType::VEC2: return 8;
        case UniformType::VEC3: return 12;
        default: return 0;
        }
    }

public:
    Uniform(std::unordered_map<std::string, UniformType> map) : map(map) {}

    vk::DescriptorSetLayoutBinding getBinding();

    
};

}