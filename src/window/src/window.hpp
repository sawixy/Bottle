#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include "surface.hpp"
#include <functional>
#include "rendercontext.hpp"
#include "window.hpp"

using RenderCallback = std::function<void(const RenderContext&)>;

class Window {
    GLFWwindow* window;
    RenderCallback renderCallback;
    RenderContext renderContext{};

public:
    Window(const char* window_name, uint32_t width, uint32_t height, RenderCallback callback);
    Surface getSurface(GraphicsAPI api, SurfaceCreate surfaceCreate);
    void update();
};