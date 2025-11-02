#pragma once

#include <vector>
#include "shader.hpp"
#include <memory>
#include <stdexcept>
#include "../const.h"

// forward declaration
class App;

class PipelineCreate {
private:
    Render* _render;
    std::vector<Shader> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> stages{};
    const std::vector<VkDynamicState> DynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    VkPipelineDynamicStateCreateInfo dynamicStates{};
    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    VkPipelineLayoutCreateInfo layoutCreateInfo{};
    VkPipelineLayout pipelineLayout;
    VkGraphicsPipelineCreateInfo createInfo{};

public:
    PipelineCreate() = default;
    PipelineCreate(
        Render* render,
        std::vector<Shader> pipelineShaders,
        VkRenderPass renderPass,
        VkPipelineViewportStateCreateInfo* pipelineViewportStateCreateInfo = nullptr,
        VkPipelineVertexInputStateCreateInfo* pipelineVertexInputStateCreateInfo = nullptr,
        VkPipelineDynamicStateCreateInfo* pipelineDynamicStates = nullptr,
        VkPipelineRasterizationStateCreateInfo* pipelineRasterizationStateCreateInfo = nullptr,
        VkPipelineInputAssemblyStateCreateInfo* pipelineInputAssemblyStateCreateInfo = nullptr,
        VkPipelineMultisampleStateCreateInfo* pipelineMultisampleStateCreateInfo = nullptr,
        VkPipelineColorBlendStateCreateInfo* pipelineColorBlendStateCreateInfo = nullptr,
        VkPipelineLayoutCreateInfo* pipelineLayoutCreateInfo = nullptr
    );

    ~PipelineCreate();

    void createPipeline(VkPipeline* pipeline);
};