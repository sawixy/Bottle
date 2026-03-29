cmake_minimum_required(VERSION 3.12)

project(bottle)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE 
    src/core/window/glfw/glfwWindowSystem.cpp
    src/core/resources/render/vulkan/VulkanShaderResource.cpp
    src/core/render/vulkan/queuemanager.cpp
    src/core/render/vulkan/context/devices.cpp
    src/core/render/vulkan/context/instance.cpp
    src/main.cpp
)

find_package(glfw3)

target_link_libraries(
    glfw
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCE})