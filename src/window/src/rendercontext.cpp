#include "rendercontext.hpp"

void VulkanRenderContext::update() {
    currentFrame = (currentFrame + 1) % max_frames_in_flight;
}