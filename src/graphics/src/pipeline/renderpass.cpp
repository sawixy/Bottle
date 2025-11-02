#include "app.hpp"
#include <iostream>

void App::createRenderPass(){
    std::cout << "Creating Render Pass" << std::endl;

    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = format.format;                            // swapchain image format
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;         // layout before render pass begins
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;     // layout after render pass ends
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;              // load operation (clear the color attachment)
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;            // store operation (store the color attachment to memory)
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   // stencil load operation (don't care if you don't have stencil buffer, stencil is used for depth testing)
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // stencil store operation (don't care)
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;                   // number of samples used for rasterization (antialiasing)

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;                                      // index of the attachment
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;   // layout of the attachment

    // subpass description (only color attachment is used)
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;       // pipeline bind point
    subpassDescription.colorAttachmentCount = 1;                                  // number of color attachments
    subpassDescription.pColorAttachments = &colorAttachmentReference;             // color attachment reference

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;                             // source subpass
    subpassDependency.dstSubpass = 0;                                               // destination subpass
    subpassDependency.srcAccessMask = 0;                                            // source access mask
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // destination access mask
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // source stage mask
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // destination stage mask

    VkRenderPassCreateInfo renderpassCreateInfo{};
    renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassCreateInfo.attachmentCount = 1;                                     // number of attachments
    renderpassCreateInfo.pAttachments = &colorAttachmentDescription;              // color attachment description
    renderpassCreateInfo.subpassCount = 1;                                        // number of subpasses
    renderpassCreateInfo.pSubpasses = &subpassDescription;                        // subpass description
    renderpassCreateInfo.dependencyCount = 0;                                     // number of dependencies
    renderpassCreateInfo.pDependencies = &subpassDependency;                      // subpass dependency

    if (vkCreateRenderPass(device, &renderpassCreateInfo, nullptr, &renderpass) != VK_SUCCESS){
        throw std::runtime_error("Failed to create render pass");
    }

    std::cout << "\tRender Pass Created" << std::endl << std::endl;
}