#pragma once

#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/queuemanager.hpp>
#include <core/render/vulkan/context.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <core/render/uniform.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderer {
private:
    Context ctx{};
    QueueManager queueManager{ctx};
    Uniform uniform;

    // TODO: Collect Swapchain to a class
    vk::raii::SwapchainKHR swapchain{nullptr};
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    vk::SurfaceKHR surface;

    vk::PresentModeKHR presentMode;
    vk::Extent2D rect;
    vk::SurfaceFormatKHR format;

    uint32_t framesInFlight;

    std::vector<vk::raii::Semaphore> imageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> fences;

    vk::raii::CommandPool graphicsPool{nullptr};
    std::vector<vk::raii::CommandBuffer> cmds;

public:
    VulkanRenderer() {
        queueManager.createQueues();
        initSwapchain();
        createImages();
        framesInFlight = images.size();  // Match the actual number of swapchain images
        initCommandBuffers();

        // Create semaphores for each frame
        for (uint32_t i = 0; i < framesInFlight; i++) {
            imageAvailableSemaphores.push_back(vk::raii::Semaphore(ctx.getDevice(), vk::SemaphoreCreateInfo()));
            renderFinishedSemaphores.push_back(vk::raii::Semaphore(ctx.getDevice(), vk::SemaphoreCreateInfo()));
        }
        
        // Create fences for frame pacing
        for (uint32_t i = 0; i < framesInFlight; i++) {
            fences.push_back(vk::raii::Fence(ctx.getDevice(), vk::FenceCreateInfo {vk::FenceCreateFlagBits::eSignaled}));
        }
    }

    void initSwapchain();
    void createImages();
    void initCommandBuffers();

    Context& getContext() { return ctx; }
    QueueManager getQueueManager() { return queueManager; }
    vk::Format getFormat() { return format.format; }
    Uniform& getUniform() { return uniform; }

    void render(std::vector<RenderComponent*>& components);
};

}