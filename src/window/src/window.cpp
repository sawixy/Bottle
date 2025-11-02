#include "window.hpp"

Window::Window(const char* window_name, uint32_t width, uint32_t height, RenderCallback callback){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwCreateWindow(width, height, window_name, NULL, NULL);

    renderCallback = callback;
}

Surface Window::getSurface(GraphicsAPI api, SurfaceCreate surfaceCreate){
    Surface surface{};
    if (api == GraphicsAPI::VULKAN){
        surfaceCreate.createSurface(api, &surface);
    }
    return surface;
}

void Window::update(){
    if (glfwWindowShouldClose(window)){
        glfwTerminate();
    }

    renderCallback(renderContext);
    renderContext.update();
}