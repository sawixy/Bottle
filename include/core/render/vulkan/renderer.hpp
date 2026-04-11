#pragma once

#include <core/render/vulkan/vulkanRenderComponent.hpp>
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/queuemanager.hpp>
#include <core/render/vulkan/context.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

#define FRAMES_IN_FLIGHT 3

namespace bottle::core::render::vulkan {

class VulkanRenderer {
private:
    Context ctx{};
    QueueManager queueManager{ctx};

    // TODO: Collect Swapchain to a class
    vk::raii::SwapchainKHR swapchain{nullptr};
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    vk::SurfaceKHR surface;

    vk::PresentModeKHR presentMode;
    vk::Extent2D rect;
    vk::SurfaceFormatKHR format;

    std::vector<vk::raii::Semaphore> renderReady;
    std::vector<vk::raii::Semaphore> presentReady;
    std::vector<vk::raii::Fence> fences;

    vk::raii::CommandPool graphicsPool{nullptr};
    vk::raii::CommandBuffers cmds{nullptr};

public:
    VulkanRenderer() {
        queueManager.createQueues();
        initSwapchain();
        createImages();
    }

    void initSwapchain();
    void createImages();
    void initCommandBuffers();

    Context& getContext() { return ctx; }
    QueueManager getQueueManager() { return queueManager; }

    void render(const std::vector<VulkanRenderComponent*>& components);
};

}