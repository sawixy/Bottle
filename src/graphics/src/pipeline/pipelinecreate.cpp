#include "shader.hpp"
#include "pipelinecreate.hpp"
#include "../render.hpp"
#include <iostream>

PipelineCreate::PipelineCreate(
    Render* render,
    std::vector<Shader> pipelineShaders,
    VkRenderPass renderPass,
    VkPipelineViewportStateCreateInfo* pipelineViewportStateCreateInfo,
    VkPipelineVertexInputStateCreateInfo* pipelineVertexInputStateCreateInfo,
    VkPipelineDynamicStateCreateInfo* pipelineDynamicStates,
    VkPipelineRasterizationStateCreateInfo* pipelineRasterizationStateCreateInfo,
    VkPipelineInputAssemblyStateCreateInfo* pipelineInputAssemblyStateCreateInfo,
    VkPipelineMultisampleStateCreateInfo* pipelineMultisampleStateCreateInfo,
    VkPipelineColorBlendStateCreateInfo* pipelineColorBlendStateCreateInfo,
    VkPipelineLayoutCreateInfo* pipelineLayoutCreateInfo
){
    // initiazation create info
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;

    shaders = pipelineShaders;

    _render = render;

    // Stages
    for (int i = 0; i < shaders.size(); i++){
        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = shaders[i].bits;
        stage.module = shaders[i].shadermodule;
        stage.pName = "main";

        if (stage.module == VK_NULL_HANDLE) {
                throw std::runtime_error("Shader module is VK_NULL_HANDLE on fragment stage: " + std::to_string(stage.stage));
            }

        stages.push_back(stage);
    }

    createInfo.pStages = stages.data();
    createInfo.stageCount = stages.size();

    std::cout << "\tStages created" << std::endl;

    // Vertex Input State (format of raw verticies data)
    if (pipelineVertexInputStateCreateInfo != nullptr){
        createInfo.pVertexInputState = pipelineVertexInputStateCreateInfo;
    } else {
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.pVertexAttributeDescriptions = nullptr;
        vertexInputState.pVertexBindingDescriptions = nullptr;
        vertexInputState.vertexAttributeDescriptionCount = 0;
        vertexInputState.vertexBindingDescriptionCount = 0;
        createInfo.pVertexInputState = &vertexInputState;
    }

    std::cout << "\tVertex Input State created" << std::endl;

    // Input Assembly State (what kind of geometry will be drawn from the vertices and if primitive restart should be enabled)
    if (pipelineInputAssemblyStateCreateInfo != nullptr){
        createInfo.pInputAssemblyState = pipelineInputAssemblyStateCreateInfo;
    } else {
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // primitive topology (points, lines, triangles, etc.)
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;              // if true, allows restarting of primitives
        createInfo.pInputAssemblyState = &inputAssemblyState;              // input assembly state
    }

    std::cout << "\tInput Assembly State created" << std::endl;

    // Viewport and Scissors (Viewport is view box, Scissor is section of viewport)
    if (pipelineViewportStateCreateInfo != nullptr){
        createInfo.pViewportState = pipelineViewportStateCreateInfo;
    } else {
        // viewport
        VkViewport viewport{};
        viewport.width = static_cast<float>(render->extent.width);
        viewport.height = static_cast<float>(render->extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        viewport.x = 0.0f;
        viewport.y = 0.0f;

        // scissor
        VkRect2D scissor{};
        scissor.extent = render->extent;
        scissor.offset = {};

        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        createInfo.pViewportState = &viewportState;
    }

    std::cout << "\tViewport and Scissors created" << std::endl;

    // Dynamic States (dynamic states are states that can be changed at runtime)
    if (pipelineDynamicStates != nullptr){
        createInfo.pDynamicState = pipelineDynamicStates;
    } else {
        dynamicStates.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStates.pDynamicStates = DynamicStates.data();
        dynamicStates.dynamicStateCount = DynamicStates.size();
        createInfo.pDynamicState = &dynamicStates;
    }

    std::cout << "\tDynamic States created" << std::endl;

    // Rasterization State (rasterization is the process of turning the geometry into fragments to be colored)
    if (pipelineRasterizationStateCreateInfo != nullptr){
        createInfo.pRasterizationState = pipelineRasterizationStateCreateInfo;
    } else {
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.depthClampEnable = VK_FALSE;         // if true, fragments beyond near and far planes are clamped to them
        rasterizationState.lineWidth = 1.0f;                    // if geometry is not a point, line or triangle, line width is used to determine width of line
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;    // culling mode (front, back or none)
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; // order of vertex rasterization
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;  // polygon mode (fill, line or point)
        rasterizationState.depthBiasEnable = VK_FALSE;          // enables depth bias (it can be used to avoid z-fighting)
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;  // if true, geometry is not drawn
        createInfo.pRasterizationState = &rasterizationState;
    }

    std::cout << "\tRasterization State created" << std::endl;

    // Multisample State (multisampling is used to reduce the amount of aliasing artifacts)
    if (pipelineMultisampleStateCreateInfo != nullptr){
        createInfo.pMultisampleState = pipelineMultisampleStateCreateInfo;
    } else {
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;     // number of samples used for rasterization
        multisampleState.sampleShadingEnable = VK_FALSE;                   // if true, enables sample shading
        multisampleState.pSampleMask = nullptr;                            // sample mask (no one yet)
        createInfo.pMultisampleState = &multisampleState;
    }

    std::cout << "\tMultisample State created" << std::endl;

    // Color blending (how to blend colors)
    if (pipelineColorBlendStateCreateInfo != nullptr){
        createInfo.pColorBlendState = pipelineColorBlendStateCreateInfo;
    } else {
        // Color Attachment State
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;  // color components to write
        colorBlendAttachmentState.blendEnable = VK_FALSE;                                                                                                      // blending is enabling
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;                                                                                   // source alpha blending factor (this shows how to blend alpha channel)
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;                                                                                  // destination alpha blending factor
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;                                                                                   // source color blending factor (this shows how to blend color)
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;                                                                                  // destination color blending factor
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;                                                                                              // color blending operation (bitwise add)
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;                                                                                              // alpha blending operation (bitwise add)

        // Color Blend State
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;                        // number of color attachments
        colorBlendState.pAttachments = &colorBlendAttachmentState;  // color attachment state
        colorBlendState.logicOpEnable = VK_FALSE;                   // enables logical operations (bitwise operations)
        colorBlendState.logicOp = VK_LOGIC_OP_COPY;                 // logical operations
        createInfo.pColorBlendState = &colorBlendState;
    }

    std::cout << "\tColor Blending State created" << std::endl;

    // Pipeline Layout
    if (pipelineLayoutCreateInfo != nullptr){
        vkCreatePipelineLayout(render->device, pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        createInfo.layout = pipelineLayout;
    } else {
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pushConstantRangeCount = 0;
        if (vkCreatePipelineLayout(render->device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline layout");
        }
        createInfo.layout = pipelineLayout;
    }

    // Render Pass
    createInfo.renderPass = renderPass;
    createInfo.subpass = 0;

    std::cout << "\tPipeline Layout created" << std::endl;
}

PipelineCreate::~PipelineCreate(){
    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_render->device, pipelineLayout, nullptr);
    }
    for (auto& shader : shaders){
        shader.cleanup();
    }
}

void PipelineCreate::createPipeline(VkPipeline* pipeline){
    if (vkCreateGraphicsPipelines(_render->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, pipeline) != VK_SUCCESS){
        throw std::runtime_error("Failed to create graphics pipeline");
    }
}