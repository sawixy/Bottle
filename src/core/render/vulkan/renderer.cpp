#include "core/render/vulkan/queuemanager.hpp"
#include "core/render/vulkan/vulkanRenderSystem.hpp"
#include "core/utils/locator.hpp"
#include "core/window/windowSystem.hpp"
#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <core/render/vulkan/renderer.hpp>
#include <vector>
#include <core/render/vulkan/vulkanRenderComponent.hpp>
#include <vulkan/vulkan_raii.hpp>

#ifdef DEBUG
#include <iostream>
#endif
#include <vulkan/vulkan_core.h>

namespace bottle::core::render::vulkan {

void VulkanRenderer::initSwapchain() {
#ifdef DEBUG
    std::cout << "Swapchain initialization started" << std::endl;
#endif
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
        if (fmt.format == vk::Format::eR8G8B8A8Srgb && fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
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

#ifdef DEBUG
    std::cout << "Swapchain initialized successfully" << std::endl;
#endif
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

void VulkanRenderer::render(std::vector<RenderComponent*>& components) {
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    static uint32_t currentFrame = 0;

    if (ctx.getDevice().waitForFences(*fences[currentFrame], vk::True, 432857092384570) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fences");
    }

    ctx.getDevice().resetFences(*fences[currentFrame]);

    auto [res, img] = swapchain.acquireNextImage(100000000, *imageAvailableSemaphores[currentFrame], nullptr);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to acquire next image from swapchain");
    }

    vk::RenderingAttachmentInfoKHR color {
        imageViews[img],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBitsKHR::eNone,
        {},
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::ClearValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}),
        nullptr
    };

    vk::RenderingAttachmentInfoKHR depth {
        imageViews[img],
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::ResolveModeFlagBitsKHR::eNone,
        {},
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::ClearValue(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f}),
        nullptr
    };

    vk::RenderingAttachmentInfoKHR stencil {
        imageViews[img],
        vk::ImageLayout::eStencilAttachmentOptimal,
        vk::ResolveModeFlagBitsKHR::eNone,
        {},
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::ClearValue(std::array<uint32_t, 4>{0, 0, 0, 0}),
        nullptr
    };

    vk::RenderingInfoKHR renderInfo {
        {},
        vk::Rect2D{ {0, 0}, rect },
        1,
        0,
        1,
        &color,
        nullptr,
        nullptr,
        nullptr
    };

    vk::CommandBufferBeginInfo cmdBeginInfo {
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        nullptr,
        nullptr
    };

#ifdef DEBUG
    std::cout << "Swapchain image count: " << images.size() << std::endl;
    std::cout << "framesInFlight: " << framesInFlight << std::endl;
    std::cout << "cmds size: " << cmds.size() << std::endl;
#endif

    // record command buffer
#ifdef DEBUG
    std::cout << "Recording command buffer for frame " << currentFrame << std::endl;
    std::cout << "Getting command buffer" << std::endl;
#endif
    vk::raii::CommandBuffer& cmd = cmds[currentFrame];

#ifdef DEBUG
    std::cout << "Beginning command buffer" << std::endl;
#endif
    try {
        cmd.begin(cmdBeginInfo);
    } catch (const vk::SystemError& err) {
#ifdef DEBUG
        std::cerr << "Failed to begin command buffer: " << err.what() << std::endl;
#endif
        throw;
    }
#ifdef DEBUG
    std::cout << "Beginning rendering" << std::endl;
#endif

    vk::ImageMemoryBarrier preBarrier {
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        images[img],
        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, nullptr, nullptr, preBarrier
    );

    cmd.beginRendering(renderInfo);

#ifdef DEBUG
    std::cout << "Swapchain format: " << vk::to_string(format.format) << std::endl;

    std::cout << "Rendering " << components.size() << " components" << std::endl;
#endif
    for (RenderComponent* component : components) {
#ifdef DEBUG
        std::cout << "Processing component: " << component << std::endl;
#endif
        auto vulkanComponent = reinterpret_cast<VulkanRenderComponentInner*>(component->getInner());
#ifdef DEBUG
        std::cout << "Getting mesh" << std::endl;
#endif

        auto mesh = vulkanComponent->getMesh();
#ifdef DEBUG
        std::cout << "Rendering component: vertices=" << mesh.vertices.size()
                  << " indices=" << mesh.indices.size()
                  << " shaders=" << component->getShaders().size() << std::endl;
        std::cout << "Binding pipeline: " << *vulkanComponent->getPipeline() << std::endl;
#endif
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanComponent->getPipeline());
        vk::raii::DescriptorSet& set = dynamic_cast<VulkanRenderSystem*>(utils::Locator::Instance().get<RenderSystem>())->getRenderer().getUniform().getSet();
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vulkanComponent->getLayout(), 0, *set, nullptr);
#ifdef DEBUG
        std::cout << "Viewport and scissor setup" << std::endl;
        std::cout << "Viewport: x=0, y=0, width=" << rect.width << ", height=" << rect.height << std::endl;
#endif
        vk::Viewport viewport {
            0.0f, 0.0f,
            static_cast<float>(rect.width), static_cast<float>(rect.height),
            0.0f, 1.0f
        };
        cmd.setViewport(0, viewport);
#ifdef DEBUG
        std::cout << "Scissor: offset=(0,0), extent=(" << rect.width << "," << rect.height << ")" << std::endl;
#endif
        vk::Rect2D scissor {
            {0, 0},
            rect
        };
        cmd.setScissor(0, scissor);
#ifdef DEBUG
        std::cout << "Binding vertex buffer" << std::endl;
#endif
        cmd.bindVertexBuffers(0, *vulkanComponent->getVertexBuffer(), {0});
#ifdef DEBUG
        std::cout << "Binding index buffer and drawing" << std::endl;
#endif
        cmd.bindIndexBuffer(*vulkanComponent->getIndexBuffer(), 0, vk::IndexType::eUint32);
#ifdef DEBUG
        std::cout << "Drawing indexed with " << vulkanComponent->getMesh().indices.size() << " indices" << std::endl;
#endif
        cmd.drawIndexed(vulkanComponent->getMesh().indices.size(), 1, 0, 0, 0);
    }

#ifdef DEBUG
    std::cout << "Ending rendering" << std::endl;
#endif

    cmd.endRendering();

    // Transition image to present layout
    vk::ImageMemoryBarrier barrier {
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::AccessFlagBits::eNone,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        images[img],
        vk::ImageSubresourceRange {
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        }
    };

    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::DependencyFlags(),
        nullptr,
        nullptr,
        barrier
    );

#ifdef DEBUG
    std::cout << "Ending command buffer" << std::endl;
#endif
    cmd.end();

#ifdef DEBUG
    std::cout << "Submitting command buffer" << std::endl;
#endif
    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo {
        1,
        &*imageAvailableSemaphores[currentFrame],
        &waitStages,
        1,
        &*cmd,
        1,
        &*renderFinishedSemaphores[img],
        nullptr
    };
    
    queueManager.getQueue(QueueManager::QueueType::GRAPHICS, 0).submit(submitInfo, fences[currentFrame]);

    vk::PresentInfoKHR presentInfo {
        1,
        &*renderFinishedSemaphores[img],
        1,
        &*swapchain,
        &img,
        nullptr
    };

    vk::Result result = queueManager.getQueue(QueueManager::QueueType::TRANSFER, 0).presentKHR(presentInfo);
#ifdef DEBUG
    std::cout << "Present result: " << vk::to_string(res) << std::endl;
#endif

    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swapchain image");
    }

    currentFrame = (currentFrame + 1) % framesInFlight;

    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> render_time = end_time - start_time;
#ifdef DEBUG
    std::cout << "Frame rendered in " << render_time.count() << " ms" << std::endl;
    std::cout << 1000.0 / render_time.count() << " FPS" << std::endl;
#endif
}

}