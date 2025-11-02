cmake_minimum_required(VERSION 3.14)
project(bottle)

file(GLOB SOURCES 
    "src/main.cpp"
)

include_directories(
    src/ecs,
    src/graphics/,
    src/window/,
    src/animation/,
)

add_subdirectory(src/graphics)

add_executable(Bottle ${SOURCES})