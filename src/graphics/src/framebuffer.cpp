#include "render.hpp"
#include <iostream>

void Render::createFramebuffers(){
    std::cout << "Creating Framebuffers" << std::endl;
    
    framebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++){
        // images, that are used for rendering
        VkImageView attachments[] = {
            swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderpass;    // link with render pass
        framebufferCreateInfo.attachmentCount = 1;        // number of attachments
        framebufferCreateInfo.pAttachments = attachments; // attachments
        framebufferCreateInfo.width = extent.width;       // width
        framebufferCreateInfo.height = extent.height;     // height
        framebufferCreateInfo.layers = 1;                 // number of layers

        if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create framebuffer");
        }
    }

    std::cout << framebuffers.size() << " framebuffers created successfully" << std::endl << std::endl;
}