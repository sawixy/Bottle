#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <vulkan/vulkan.hpp>

#include <iostream>

namespace bottle::core::window::glfw {

GLFWWindowSystem::GLFWWindowSystem() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 
    window = glfwCreateWindow(800, 800, "Bottle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
}

vk::SurfaceKHR GLFWWindowSystem::vulkanInit(VkInstance instance) {
    VkSurfaceKHR _surface;
    glfwCreateWindowSurface(instance, window, nullptr, &_surface);
    std::cout << "surface: " << _surface << std::endl;
    return vk::SurfaceKHR(_surface);
}

void GLFWWindowSystem::update() {
    glfwPollEvents();
}

GLFWWindowSystem::~GLFWWindowSystem() {
    glfwTerminate();
}

}