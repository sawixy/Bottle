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
    surface = utils::Locator::Instance().get<window::WindowSystem>()->vulkanInit(*ctx.getInstance());
    vk::SurfaceCapabilitiesKHR surfaceCaps = ctx.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    std::vector<vk::SurfaceFormatKHR> formats = ctx.getPhysicalDevice().getSurfaceFormatsKHR(surface);
    std::vector<vk::PresentModeKHR> modes = ctx.getPhysicalDevice().getSurfacePresentModesKHR(surface);
    int imageCount = FRAMES_IN_FLIGHT;
    if (surfaceCaps.maxImageCount != 0) {
        imageCount = std::clamp<uint32_t>(FRAMES_IN_FLIGHT, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);
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
    for (auto fmt : formats) {
        format = fmt;
        if (fmt.format == vk::Format::eA8B8G8R8SrgbPack32){
            break;
        }
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
        {},
        queueManager.getFamilyIndex(QueueManager::QueueType::GRAPHICS),
        nullptr
    };

    graphicsPool = std::move(ctx.getDevice().createCommandPool(poolCI));

    vk::CommandBufferAllocateInfo cmdallocCI {
        graphicsPool,
        vk::CommandBufferLevel::ePrimary,
        FRAMES_IN_FLIGHT,
        nullptr
    };

    cmds = std::move(ctx.getDevice().allocateCommandBuffers(cmdallocCI));
}

void VulkanRenderer::render(const std::vector<VulkanRenderComponent*>& components) {
    for (int i = 0; i < FRAMES_IN_FLIGHT; i++) { // FIXME: FRAMES_IN_FLIGHT might be not actual imageCount. PS. Sorry for my English
        if (ctx.getDevice().waitForFences(*fences[i], vk::True, 1000) != vk::Result::eSuccess) {
            throw std::runtime_error("Yo! Im crashed lol)");
        }
        ctx.getDevice().resetFences(*fences[i]);

        auto [result, img] = swapchain.acquireNextImage(100000, renderReady[i], fences[i]);

        // Let`s begin DYNAMIC RENDERING BOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM
        vk::Rect2D render_area = vk::Rect2D{0, rect};

        vk::ClearValue colorClearValue = vk::ClearColorValue{std::array<float, 4>{0.01, 0.01, 0.01, 0.5}};
        vk::ClearValue depthStencilClearValue = vk::ClearDepthStencilValue{1.0 , 1};

        // attachments
        vk::RenderingAttachmentInfoKHR color {
            imageViews[i],
            vk::ImageLayout::eUndefined,
            vk::ResolveModeFlagBits::eAverage,
            imageViews[i],
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            colorClearValue,
            nullptr
        };

        vk::RenderingAttachmentInfoKHR depth {
            imageViews[i],
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            nullptr,
            vk::ImageLayout::eUndefined,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eDontCare,
            depthStencilClearValue,
            nullptr
        };

        vk::RenderingAttachmentInfoKHR stencil {
            imageViews[i],
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            nullptr,
            vk::ImageLayout::eUndefined,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eDontCare,
            depthStencilClearValue,
            nullptr
        };

        vk::RenderingInfoKHR rendering {
            {},
            vk::Rect2D{{}, rect},
            1,
            1,
            1,
            &color,
            &depth,
            &stencil,
            nullptr
        };

        vk::CommandBuffer cmd = cmds[i];
        
        cmd.reset();
        if (cmd.begin({}) != vk::Result::eSuccess) {
            throw std::runtime_error("Hey! I had crashed right now) idk why :3");
        }

        cmd.beginRendering(rendering);

        for (auto &comp : components) {
            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, comp.getPipeline());
            cmd.bindVertexBuffers(0, *comp.getVertexBuffer(), {0});
            cmd.bindIndexBuffer(*comp.getIndexBuffer(), {}, vk::IndexType::eUint32);
        }

        cmd.endRendering();
        cmd.end();

        vk::SubmitInfo submitI {
            1,
            &*renderReady[i],
            nullptr,
            1,
            &cmd,
            1,
            &*presentReady[i],
            nullptr
        };

        vk::PresentInfoKHR presentI {
            {},
            &*presentReady[i],
            1,
            &*swapchain,
            reinterpret_cast<uint32_t*>(&i),
            nullptr,
            nullptr
        };

        queueManager.getQueue(QueueManager::QueueType::GRAPHICS, 0).submit(submitI);

        if (queueManager.getQueue(QueueManager::QueueType::TRANSFER, 0).presentKHR(presentI) != vk::Result::eSuccess) {
            throw std::runtime_error("Oh.. I had just crashed, you know what to do... maybe");
        }
    }
}

}