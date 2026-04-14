#include "core/render/vulkan/queuemanager.hpp"
#include "core/utils/locator.hpp"
#include "core/window/windowSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/renderer.hpp>
#include <vector>

#include <iostream>

namespace bottle::core::render::vulkan {

void VulkanRenderer::initSwapchain() {
    std::cout << "Swapchain initialization started" << std::endl;
    surface = utils::Locator::Instance().get<window::WindowSystem>()->vulkanInit(*ctx.getInstance());
    vk::SurfaceCapabilitiesKHR surfaceCaps = ctx.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    std::vector<vk::SurfaceFormatKHR> formats = ctx.getPhysicalDevice().getSurfaceFormatsKHR(surface);
    std::vector<vk::PresentModeKHR> modes = ctx.getPhysicalDevice().getSurfacePresentModesKHR(surface);
    int imageCount = 3;  // Surface requires minimum 3 images
    if (surfaceCaps.maxImageCount != 0) {
        imageCount = std::clamp<uint32_t>(3, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);
    }

    rect = vk::Extent2D{800, 800}; // TODO: Remove hardcoded numbers

    // present mode
    for (auto mode : modes) {
        presentMode = mode;
        if (mode == vk::PresentModeKHR::eMailbox) {
            break;
        }
    }

    // format
    bool foundPreferred = false;
    for (auto fmt : formats) {
        if (fmt.format == vk::Format::eA8B8G8R8SrgbPack32 && fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            format = fmt;
            foundPreferred = true;
            break;
        }
    }

    if (!foundPreferred) {
        for (auto fmt : formats) {
            if (fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                format = fmt;
                foundPreferred = true;
                break;
            }
        }
    }

    if (!foundPreferred) {
        format = formats[0];
    }

    std::vector<uint32_t> queuesIndices = {
        queueManager.getFamilyIndex(QueueManager::QueueType::GRAPHICS),
        queueManager.getFamilyIndex(QueueManager::QueueType::TRANSFER)
    };

    vk::SwapchainCreateInfoKHR swapchainCI {
        {},
        surface,
        static_cast<uint32_t>(imageCount),
        format.format,
        format.colorSpace,
        rect,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        (
            queueManager.getFamilyIndex(QueueManager::QueueType::GRAPHICS) == 
            queueManager.getFamilyIndex(QueueManager::QueueType::TRANSFER)
        ) ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        static_cast<uint32_t>(queuesIndices.size()),
        queuesIndices.data(),
        vk::SurfaceTransformFlagBitsKHR::eIdentity,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        vk::True,
        nullptr,
        nullptr
    };

    swapchain = std::move(ctx.getDevice().createSwapchainKHR(swapchainCI));

    std::cout << "Swapchain initialized successfully" << std::endl;
}

void VulkanRenderer::createImages() {
    images = swapchain.getImages();
    imageViews.reserve(images.size());

    for (int i = 0; i < images.size(); i++) {
        vk::ImageViewCreateInfo imageViewCI {
            {},
            images[i],
            vk::ImageViewType::e2D,
            format.format,
            vk::ComponentMapping {
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity
            },
            vk::ImageSubresourceRange {
                vk::ImageAspectFlagBits::eColor,
                0,
                1,
                0,
                1
            },
            nullptr
        };

        imageViews.push_back(std::move(ctx.getDevice().createImageView(imageViewCI)));
    }
}

void VulkanRenderer::initCommandBuffers() {
    vk::CommandPoolCreateInfo poolCI {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueManager.getFamilyIndex(QueueManager::QueueType::GRAPHICS),
        nullptr
    };

    graphicsPool = std::move(ctx.getDevice().createCommandPool(poolCI));

    vk::CommandBufferAllocateInfo cmdallocCI {
        graphicsPool,
        vk::CommandBufferLevel::ePrimary,
        framesInFlight,
        nullptr
    };

    cmds = std::move(ctx.getDevice().allocateCommandBuffers(cmdallocCI));
}

void VulkanRenderer::render(const std::vector<VulkanRenderComponent*>& components) {
    static uint32_t i = 0;

    // Acquire image with semaphore
    auto [result, img] = swapchain.acquireNextImage(100000, renderReady[i], nullptr);
    if (result != vk::Result::eSuccess) {
        std::cerr << "Failed to acquire image" << std::endl;
        return;
    }

    size_t imageIndex = img % images.size();

    // Let`s begin DYNAMIC RENDERING BOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM
    vk::Rect2D render_area = vk::Rect2D{0, rect};

    vk::ClearValue colorClearValue = vk::ClearColorValue{std::array<float, 4>{0.01, 0.01, 0.01, 0.5}};
    vk::ClearValue depthStencilClearValue = vk::ClearDepthStencilValue{1.0 , 1};

    // attachments
    vk::RenderingAttachmentInfoKHR color(
        *imageViews[imageIndex],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone,
        nullptr,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        colorClearValue,
        nullptr
    );

    vk::RenderingInfoKHR rendering {
        {},
        vk::Rect2D{{}, rect},
        1,
        0,
        1,
        &color,
        nullptr,
        nullptr,
        nullptr
    };

    vk::raii::CommandBuffer& cmd = cmds[i];
        
    cmd.reset();
    cmd.begin({});

    // Transition image from UNDEFINED to COLOR_ATTACHMENT_OPTIMAL
    vk::ImageMemoryBarrier barrier(
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        images[imageIndex],
        vk::ImageSubresourceRange{
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        },
        nullptr
    );
    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::DependencyFlags{},
        nullptr,
        nullptr,
        barrier
    );

    cmd.beginRendering(rendering);

    cmd.setViewport(0, vk::Viewport{
        0.0f, 0.0f,
        static_cast<float>(rect.width), static_cast<float>(rect.height),
        0.0f, 1.0f
    });
    cmd.setScissor(0, vk::Rect2D{{0, 0}, rect});

    for (auto &comp : components) {
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *comp->getPipeline());
        cmd.bindVertexBuffers(0, *comp->getVertexBuffer(), {0});
        cmd.bindIndexBuffer(*comp->getIndexBuffer(), 0, vk::IndexType::eUint32);
        cmd.drawIndexed(static_cast<uint32_t>(comp->getMesh().indices.size()), 1, 0, 0, 0);
    }

    cmd.endRendering();

    // Transition image from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
    vk::ImageMemoryBarrier presentBarrier(
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::AccessFlagBits::eNone,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        images[imageIndex],
        vk::ImageSubresourceRange{
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        },
        nullptr
    );
    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::DependencyFlags{},
        nullptr,
        nullptr,
        presentBarrier
    );

    cmd.end();

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitI {
        1,
        &*renderReady[img],
        &waitStage,
        1,
        &*cmd,
        1,
        &*presentReady[img],
        nullptr
    };

    vk::PresentInfoKHR presentI {
        1,
        &*presentReady[img],
        1,
        &*swapchain,
        &img,
        nullptr
    };

    queueManager.getQueue(QueueManager::QueueType::GRAPHICS, 0).submit(submitI, *fences[i]);
    if (queueManager.getQueue(QueueManager::QueueType::GRAPHICS, 0).presentKHR(presentI) != vk::Result::eSuccess) {
        throw std::runtime_error("Present failed");
    }

    i = (i + 1) % framesInFlight;
}

}