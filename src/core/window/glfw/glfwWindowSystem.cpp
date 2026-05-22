#include "core/config/configSystem.hpp"
#include "core/utils/locator.hpp"
#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/window/glfw/glfwWindowSystem.hpp>
#include <vulkan/vulkan.hpp>
#include <bottle.hpp>

namespace bottle::core::window::glfw {

GLFWWindowSystem::GLFWWindowSystem() {
    //glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 
    width = utils::Locator::Instance().get<config::ConfigSystem>()->get<int>("window.width");
    height = utils::Locator::Instance().get<config::ConfigSystem>()->get<int>("window.height");
    window = glfwCreateWindow(width, height, "Bottle", nullptr, nullptr);
    //window = glfwCreateWindow(800, 800, "Bottle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
}

vk::SurfaceKHR GLFWWindowSystem::vulkanInit(VkInstance instance) {
    VkSurfaceKHR _surface;
    glfwCreateWindowSurface(instance, window, nullptr, &_surface);
    return vk::SurfaceKHR(_surface);
}

void GLFWWindowSystem::update() {
    glfwPollEvents();

    if (glfwWindowShouldClose(window)) {
        throw std::runtime_error("Window closed");
    }
}

GLFWWindowSystem::~GLFWWindowSystem() {
    glfwTerminate();
}

}
