cmake_minimum_required(VERSION 3.12)

project(bottle)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE 
    src/main.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCE})