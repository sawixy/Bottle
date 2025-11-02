#include "render.hpp"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

#include "window.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data)
{
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cout << "WARNING: " << callback_data->pMessageIdName << " - " << callback_data->pMessage << std::endl;
    }
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        std::cerr << "ERROR: " << callback_data->pMessageIdName << " - " << callback_data->pMessage << std::endl;
    }
    return VK_FALSE;
}

void Render::initVulkan(){
    setupDebugMessenger();
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandBuffers();
    sync();
}

void Render::loop(){
    int currentFrame = 0;
    uint64_t framesCount = 0;

    std::cout << "Starting main loop" << std::endl << std::endl;

    while (!glfwWindowShouldClose(window)) {
        std::cout << "=== Frame " << framesCount << " ===" << std::endl;

        glfwPollEvents();

        if (currentFrame >= MAX_FRAMES_IN_FLIGHT) {
            std::cout << "ERROR: currentFrame out of bounds: " << currentFrame << std::endl;
            break;
        }

        std::cout << "Waiting for fence..." << std::endl;
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        std::cout << "Fence signaled, resetting..." << std::endl;
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        std::cout << "Acquired image index: " << imageIndex << std::endl;

        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        std::cout << "Recorded command buffer" << std::endl;

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = waitStages;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        if (vkQueuePresentKHR(presentQueue, &presentInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swapchain image");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        framesCount++;
    }
    vkDeviceWaitIdle(device);
}

void Render::createInstance() {
    // GLFW Extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    if (glfw_extension_count == 0) {
        throw std::runtime_error("Failed to get required GLFW extensions");
    }

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Render settings
    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.apiVersion = VK_API_VERSION_1_4;
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "Bottle Grpahics Engine";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pApplicationName = "Bottle Graphics Engine";
    
    // Debug messenger create info
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debug_utils_messenger_callback;

    // Creating instance
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.pApplicationInfo = &appinfo;
    instanceCreateInfo.pNext = &debugCreateInfo;
    
    // Validation layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::cout << "Available layers:" << std::endl;
    for (const auto& layer : availableLayers) {
        std::cout << "\t" << layer.layerName << std::endl;
    }

    bool validationFound = false;
    for (const auto& layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layer : availableLayers) {
            if (strcmp(layer.layerName, layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (layerFound) {
            std::cout << "Found layer: " << layerName << std::endl;
            validationFound = true;
        } else {
            std::cout << "Missing layer: " << layerName << std::endl;
        }
    }

    instanceCreateInfo.enabledLayerCount = validationFound ? static_cast<uint32_t>(validationLayers.size()) : 0;
    instanceCreateInfo.ppEnabledLayerNames = validationFound ? validationLayers.data() : nullptr;
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.pNext = validationFound ? (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo : nullptr;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create VkInstance");
    }

    std::cout << "Instance created successfully!" << std::endl;
}

void Render::setupDebugMessenger(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool validationFound = false;
    for (const auto& layer : availableLayers){
        if (strcmp(layer.layerName, validationLayers[0]) == 0){
            validationFound = true;
            break;
        }
    }

    if (!validationFound) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debug_utils_messenger_callback;
    
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, &createInfo, nullptr, &debugMessenger);
    }

    std::cout << "Debug Messenger setuped" << std::endl;
}

void Render::createSurface(){
    window.getWindowSurface(instance, &surface);
}

Render::~Render(){
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
    }

    if (device != VK_NULL_HANDLE) {
        if (!commandBuffers.empty() && commandPool != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
            commandBuffers.clear();
        }

        if (commandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device, commandPool, nullptr);
            commandPool = VK_NULL_HANDLE;
        }

        for (auto semaphore : imageAvailableSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }
        imageAvailableSemaphores.clear();

        for (auto semaphore : renderFinishedSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }
        renderFinishedSemaphores.clear();

        for (auto fence : inFlightFences) {
            if (fence != VK_NULL_HANDLE) {
                vkDestroyFence(device, fence, nullptr);
            }
        }
        inFlightFences.clear();

        for (auto framebuffer : framebuffers) {
            if (framebuffer != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
        }
        framebuffers.clear();

        if (pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, pipeline, nullptr);
        }
        
        if (renderpass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(device, renderpass, nullptr);
        }
        
        for (auto imageView : swapchainImageViews) {
            if (imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(device, imageView, nullptr);
            }
        }
        swapchainImageViews.clear();
        
        if (swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device, swapchain, nullptr);
        }
        
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    if (debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, nullptr);
        }
        debugMessenger = VK_NULL_HANDLE;
    }

    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    glfwTerminate();
}