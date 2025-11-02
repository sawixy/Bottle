#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "graphicsapi.hpp"

class Surface {
public:
    // Surface
    VkSurfaceKHR vulkanSurface;

    void createVulkanSurface(VkInstance instance, GLFWwindow* window) ;
};

class SurfaceCreate {
private:
    GLFWwindow* window;
public:
    VkInstance instance;

    SurfaceCreate(GLFWwindow* window) : window(window) {};
    void createSurface(GraphicsAPI api, Surface* surface);
};