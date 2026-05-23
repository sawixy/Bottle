#pragma once

#include "vulkan/vulkan.hpp"
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/render/vulkan/context.hpp>

namespace bottle::core::render {

struct UniformUnit {
    std::string name;
    uint offset;      // For Vulkan
};

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
    /* For Vulkan */
    uint last_offset = 0;
    void* data;
    vk::raii::Buffer buf{nullptr};
    vk::raii::DeviceMemory mem{nullptr};
    vk::raii::DescriptorSet descSet{nullptr};
    vk::raii::DescriptorSetLayout setLayout{nullptr};
    vk::raii::DescriptorPool pool{nullptr};
    vk::DescriptorSetLayoutBinding binding {
        0,  // Default binding
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        nullptr
    };

    std::unordered_map<std::string, UniformUnit> map;

    static size_t getSize(UniformType type) {
        switch (type) {
        case UniformType::UINT: return 4;
        case UniformType::INT: return 4;
        case UniformType::FLOAT: return 4;
        case UniformType::MAT3: return 36;
        case UniformType::MAT4: return 64;
        case UniformType::VEC2: return 8;
        case UniformType::VEC3: return 12;
        default: return 0;
        }
    }

public:
    Uniform(std::unordered_map<std::string, UniformType> typemap) {
        for (auto [name, type] : typemap) {
            map[name] = UniformUnit {
                .name = name,
                .offset = last_offset
            };
            last_offset += getSize(type);
        }
    }
    Uniform() {}

    void addType(std::string name, UniformType type) {
        map[name] = UniformUnit {
            .name = name,
            .offset = last_offset
        };
        last_offset += getSize(type);
    }

    /* For Vulkan */
    vk::DescriptorSetLayoutBinding getBinding() { return binding; }
    void initBuffer();
    vk::raii::DescriptorSet& getSet() { return descSet; }
    vk::raii::DescriptorPool& getPool() { return pool; }
    vk::raii::DescriptorSetLayout& getSetLayout() { return setLayout; }

    void set(std::string name, uint value);
    void set(std::string name, int value);   
    void set(std::string name, float value);
    void set(std::string name, glm::vec2 value);
    void set(std::string name, glm::vec3 value);
    void set(std::string name, glm::mat3 value);
    void set(std::string name, glm::mat4 value);

    ~Uniform() {
        delete[] static_cast<char*>(data);
    }
};

}