cmake_minimum_required(VERSION 3.20)
project(Bottle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

add_executable(bottle ${SOURCES})

target_include_directories(bottle PRIVATE include)

target_compile_definitions(bottle PRIVATE 
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    DEBUG=1
)

target_link_libraries(bottle 
    Vulkan::Vulkan
    glfw
    dl
    pthread
)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found!")
endif()