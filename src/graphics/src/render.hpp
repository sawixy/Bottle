#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "pipeline.hpp"
#include "src/window.hpp"

#include "const.h"

class Render{
public:
    Window* window;                                    // Window
    VkInstance instance{};                             // Vulkan runtime
    VkDebugUtilsMessengerEXT debugMessenger;           // debug
    VkSurfaceKHR surface;                              // surface
    VkPhysicalDevice physicalDevice{};                 // physical device
    VkDevice device;                                   // logical device
    VkSurfaceFormatKHR format;                         // format
    VkSwapchainKHR swapchain{};                        // swapchain
    std::vector<VkImage> swapchainImages{};            // For images
    std::vector<VkImageView> swapchainImageViews{};    // For image views
    std::vector<VkFramebuffer> framebuffers{};         // framebuffers
    VkExtent2D extent{};                               // extent (surface size)
    VkViewport viewport{};                             // viewport
    VkRect2D scissor{};                                // scissor
    VkPipelineViewportStateCreateInfo viewportState;   // viewport
    VkPipeline pipeline;                               // graphics pipeline
    VkRenderPass renderpass;                           // render pass (recipe of making images)
    std::vector<VkCommandBuffer> commandBuffers{};     // command buffers
    VkCommandPool commandPool;                         // command pool
    std::vector<VkSemaphore> imageAvailableSemaphores; // semaphores (sync threads that avilable to draw)
    std::vector<VkSemaphore> renderFinishedSemaphores; // semaphores (sync threads that finished render)
    std::vector<VkFence> inFlightFences;               // fences (sync cpu with gpu) 

    uint32_t graphicsQueueFamilyIndex;                 // thread that can draw
    uint32_t presentQueueFamilyIndex;                  // thread that can present

    PipelineCreate* pipelineCreate;                    // pipeline creater

    VkQueue graphicsQueue;                             // graphics queue
    VkQueue presentQueue;                              // present queue

    Render(Window* window) : window(window) {}
    
    // Validation layers
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    void run(){
        initVulkan();
        loop();
    }

    // cleanup
    ~Render();

    void initVulkan();

    void loop();

    // For Vulkan initialization
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandBuffers();
    void sync();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};