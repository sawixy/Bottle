#pragma once

#include <GLFW/glfw3.h>

#include "core/window/windowSystem.hpp"
#include <core/window/windowSystem.hpp>

namespace bottle::core::window::glfw {

class GLFWWindowSystem : public WindowSystem {
private:
    GLFWwindow* window;

public:
    GLFWWindowSystem();

    vk::SurfaceKHR vulkanInit(VkInstance instance) override;

    void update() override;
};

}