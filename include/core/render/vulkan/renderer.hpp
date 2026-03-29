#pragma once

#include "vulkan/vulkan.hpp"
#include <core/render/vulkan/queuemanager.hpp>
#include <core/render/vulkan/context.hpp>
#include <core/render/renderSystem.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {

class VulkanRenderer {
private:
    Context ctx{};
    QueueManager queueManager{ctx};

    // TODO: Collect Swapchain to a class
    vk::raii::SwapchainKHR swapchain{nullptr};
    std::vector<vk::raii::Image> images;
    std::vector<vk::raii::ImageView> imageViews;

    vk::PresentModeKHR presentMode;
    vk::Extent2D rect;
    vk::Format format;

    std::vector<vk::raii::Semaphore> renderReady;
    std::vector<vk::raii::Semaphore> presentReady;
    vk::raii::Fence fence{nullptr};

public:
    VulkanRenderer() {}

    Context& getContext() { return ctx; }

    void addMesh(Mesh mesh);

    void render();
};

}