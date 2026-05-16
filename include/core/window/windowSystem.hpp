#pragma once

#include <core/utils/ecs/system.hpp>
#include <vulkan/vulkan.hpp>

namespace bottle::core::window {

class WindowSystem : public utils::System {
protected:
    uint32_t width;
    uint32_t height;

public:
    virtual vk::SurfaceKHR vulkanInit(VkInstance instance) = 0;
    virtual std::vector<const char*> getRequiredVulkanExtensions() const = 0;
    virtual void update() override = 0;
    virtual uint32_t getWidth() const { return width; }
    virtual uint32_t getHeight() const { return height; }
};

}