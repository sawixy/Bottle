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

    std::vector<const char*> getRequiredVulkanExtensions() const override {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    }

    void update() override;

    ~GLFWWindowSystem();
};

}