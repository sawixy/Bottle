#include "shader.hpp"
#include <fstream>
#include "../render.hpp"

Shader::Shader(Render* render, std::string path, ShaderType bits){
    _render = render;
    this->bits = (VkShaderStageFlagBits)bits;
    if (path.substr(path.find_last_of(".") + 1) == "spv"){
        this->path = path;
    } else {
        throw std::runtime_error("File is not a SPV file");
    }
    this->readFile();
    this->createShaderModule();
}

void Shader::createShaderModule(){
    if (code.size() > 0){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(_render->device, &createInfo, nullptr, &shadermodule) != VK_SUCCESS){}
    } else {
        throw std::runtime_error("Code is empty");
    }
}

void Shader::readFile(){
    if (std::ifstream file{path, std::ios::ate | std::ios::binary}){
        size_t fileSize = (size_t)file.tellg();
        code.resize(fileSize);
        file.seekg(0);
        file.read(code.data(), fileSize);
        file.close();
    } else {
        if (path == ""){
            throw std::runtime_error("Path is empty");
        } else {
            throw std::runtime_error("File not found");
        }
    }
}

void Shader::cleanup(){
    if (shadermodule != VK_NULL_HANDLE){
        vkDestroyShaderModule(_render->device, shadermodule, nullptr);
    }
}