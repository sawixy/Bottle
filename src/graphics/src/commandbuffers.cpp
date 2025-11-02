#include <iostream>
#include "render.hpp"

void Render::createCommandBuffers(){
    std::cout << "Creating Command Buffers" << std::endl;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    std::cout << "Command Buffers created successfully" << std::endl << std::endl;
}

void Render::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
    vkResetCommandBuffer(commandBuffer, 0);

    if (imageIndex >= framebuffers.size()) {
        std::cout << "ERROR: Framebuffer index out of bounds!" << std::endl;
        return;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    VkClearValue clearValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass;
    renderPassInfo.pClearValues = &clearValue;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    std::cout << "viewport: " << "x: " << viewportState.pViewports->x << " y: " << viewportState.pViewports->y << " width: " << viewportState.pViewports->width << " height: " << viewportState.pViewports->height << std::endl;

    vkCmdSetViewport(commandBuffer, 0, viewportState.viewportCount, viewportState.pViewports);
    vkCmdSetScissor(commandBuffer, 0, viewportState.scissorCount, viewportState.pScissors);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
}
