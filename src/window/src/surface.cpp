#include "surface.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

void Surface::createVulkanSurface(VkInstance instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &vulkanSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}