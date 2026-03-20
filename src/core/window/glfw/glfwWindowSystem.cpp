#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <vulkan/vulkan.hpp>

namespace bottle::core::window::glfw {

GLFWWindowSystem::GLFWWindowSystem() {
    window = glfwCreateWindow(800, 800, "Bottle", nullptr, nullptr);
}

vk::SurfaceKHR GLFWWindowSystem::vulkanInit(VkInstance instance) {
    VkSurfaceKHR _surface;
    glfwCreateWindowSurface(instance, window, nullptr, &_surface);
    return vk::SurfaceKHR(_surface);
}

}