#pragma once

#include <cstdint>

class RenderContext {
public:
    virtual void update() {}
};

class VulkanRenderContext : public RenderContext {
private:
    uint32_t currentFrame = 0;
    uint32_t max_frames_in_flight = 0;
public:
    VulkanRenderContext(uint32_t max_frames_in_flight) : max_frames_in_flight(max_frames_in_flight){}
    void update() override;
};