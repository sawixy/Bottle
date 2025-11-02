#include "render.hpp"
#include <stdexcept>
#include <iostream>

void Render::createSwapchain(){
    std::cout << "Creating Swapchain" << std::endl;

    // Get surface capabilities
    VkSurfaceCapabilitiesKHR capatibilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capatibilities);

    // Creating Extent
    if (capatibilities.currentExtent.width != UINT32_MAX){
        extent = capatibilities.currentExtent;
    } else {
        extent = VkExtent2D{static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT)};
    }

    // Get all formats (Color sRGB, RGB, RGA)
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats{formatCount};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    // Get all present Modes
    uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes{presentModesCount};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data());

    // Choosing format
    format = formats[0]; // fallback
    for (int i = 0; i < formatCount; i++){
        VkSurfaceFormatKHR currentFormat = formats[i];
        if (currentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && // SRGB
            currentFormat.format == VK_FORMAT_B8G8R8A8_SRGB){                // SRGB
            format = currentFormat;
            break;
        }
    }

    std::cout << "\tFormat selected" << std::endl;

    // Choosing present Mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // fallback
    for (int i = 0; i < presentModesCount; i++){
        VkPresentModeKHR currentPresentMode = presentModes[i];
        if (currentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR){ // MAILBOX is better then FIFO (doesnt metter render if queue is full)
            presentMode = currentPresentMode;
        }
    }

    std::cout << "\tPresent Mode selected" << std::endl;

    std::cout << "\tExtent: " << extent.width << "x" << extent.height << std::endl;
    std::cout << "\tMin image count: " << capatibilities.minImageCount << std::endl;
    std::cout << "\tMax image count: " << capatibilities.maxImageCount << std::endl;

    std::cout << "\tSupported usage flags: " << capatibilities.supportedUsageFlags << std::endl;
    std::cout << "\tOur usage flags: " << VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT << std::endl;

    // Creating Swapchain
    // Queues indices Array
    uint32_t queuesIndices[] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};

    // Is queues different 
    bool queuesAreDifferent = graphicsQueueFamilyIndex != presentQueueFamilyIndex;

    std::cout << "\tCreating Swapchain create info" << std::endl;

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.imageExtent = extent;                                // extent
    swapchainCreateInfo.imageFormat = format.format;                         // format
    swapchainCreateInfo.imageColorSpace = format.colorSpace;                 // color space (sRGB, RGB...)
    swapchainCreateInfo.imageArrayLayers = 1;                                // array layers
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    // image usage
    
    if (queuesAreDifferent) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;   // concurrent for different queue
        swapchainCreateInfo.queueFamilyIndexCount = 2;       
        swapchainCreateInfo.pQueueFamilyIndices = queuesIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;    // exclusive for one queue     
    }

    swapchainCreateInfo.oldSwapchain = nullptr;                              // for inheritance
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // use alpha channel for blending window with other windows
    swapchainCreateInfo.surface = surface;                                   // surface
    swapchainCreateInfo.preTransform = capatibilities.currentTransform;      // transform (like rotation by 90 degres)
    swapchainCreateInfo.presentMode = presentMode;                           // present mode
    swapchainCreateInfo.clipped = VK_TRUE;                                   // Clip non visible part
    
    // Checking maxImageCount
    uint32_t imageCount = capatibilities.minImageCount + 1;
    if (capatibilities.maxImageCount > 0 && imageCount > capatibilities.maxImageCount) {
        imageCount = capatibilities.maxImageCount;
    }
    swapchainCreateInfo.minImageCount = imageCount; // Min images count in swapchain

    VkResult result;
    if ((result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain)) != VK_SUCCESS){
        std::cout << "Failed to create Swapchain (code: " << result << ")" << std::endl;
        throw std::runtime_error("Failed to create Swapchain");
    }

    // Get Swapchain images
    uint32_t swapchainImagesCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImagesCount, nullptr);
    swapchainImages.resize(swapchainImagesCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImagesCount, swapchainImages.data());
    swapchainImageViews.resize(swapchainImagesCount);

    std::cout << "\tSwapchain images successfuly retrieved to vector" << std::endl;

    std::cout << "Swapchain created successfully" << std::endl << std::endl;
}

void Render::createImageViews(){
    std::cout << "Creating Image Views" << std::endl;

    // Image View for ever Image
    for (int i = 0; i < swapchainImages.size(); i++){
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;                    // swizzle identity
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;                    // swizzle identity
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;                    // swizzle identity
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;                    // swizzle identity
        imageViewCreateInfo.format = format.format;                                   // format
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // aspect mask (working with color now)
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;                        // no mip mapping
        imageViewCreateInfo.subresourceRange.levelCount = 1;                          // first level
        imageViewCreateInfo.subresourceRange.layerCount = 1;                          // only one layer
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;                      // base of layers
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;                         // view type (3D or 2D.. or 1D)
        imageViewCreateInfo.image = swapchainImages[i];                               // image for ImageView
        VkResult result;
        if ((result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i])) != VK_SUCCESS){
            std::cout << "[" << i << "]" << "Failed to create ImageView (code: " << result << ")" << std::endl;;
            throw std::runtime_error("Failed to create ImageView");
        } else {
            std::cout << "\tImageView " << i << " created successfully" << std::endl;
        }
    }
    std::cout << "All ImageViews created successfully" << std::endl << std::endl;
}