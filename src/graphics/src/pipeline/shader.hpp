#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vector>

// forward declaration
class Render;

enum ShaderType {
    VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
    FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
    GEOMETRY = VK_SHADER_STAGE_GEOMETRY_BIT,
    COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT
};

class Shader{
private:
    Render* _render;
    std::vector<char> code{};
    std::string path;
    void readFile();
    void createShaderModule();

public:
    VkShaderStageFlagBits bits;
    VkShaderModule shadermodule;

    Shader(Render* render, std::string path, ShaderType bits);
    void cleanup();
};