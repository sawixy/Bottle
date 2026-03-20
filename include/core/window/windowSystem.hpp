#pragma once

#include <core/utils/ecs/system.hpp>
#include <vulkan/vulkan.hpp>

namespace bottle::core::window {

class WindowSystem : public utils::System {
public:
    virtual vk::SurfaceKHR vulkanInit(VkInstance instance) = 0 ;
    virtual void update() override = 0;
};

}