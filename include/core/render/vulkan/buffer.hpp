#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {

class Buffer {
private:
    vk::raii::Buffer buffer{nullptr};
    vk::raii::DeviceMemory memory{nullptr};
    int size = 0;

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

public:
    Buffer(size_t size, vk::BufferUsageFlags usage);

    void* getMappedMemory();
    vk::raii::Buffer& getBuffer() { return buffer; }
    vk::raii::DeviceMemory& getMemory() { return memory; }
    size_t getSize() { return size; }

    ~Buffer();
};

}