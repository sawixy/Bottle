#include "shader.hpp"
#include "pipelinecreate.hpp"
#include <iostream>
#include <memory>
#include "../render.hpp"

void Render::createGraphicsPipeline(){
    std::cout << "Creating Graphics Pipeline" << std::endl;

    std::vector<Shader> shaders{
        Shader(this, "shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
        Shader(this, "shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
    };

    std::cout << "\tShaders loaded" << std::endl;

    // viewport
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    // scissor
    scissor.extent = extent;
    scissor.offset = {};

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    try {
        pipelineCreate = new PipelineCreate{this, shaders, renderpass};
        pipelineCreate->createPipeline(&pipeline);
    } catch (std::runtime_error e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Graphics Pipeline successfully created" << std::endl << std::endl;
}