#pragma once

#include <core/render/vulkan/context.hpp>

namespace bottle::core::render::vulkan {

class DescriptorEntry {
private:
    std::vector<uint32_t> bindings;
    vk::raii::DescriptorSet set;

public:
    uint32_t getBinding(int index) {
        return bindings[index];
    }
};

class DescriptorManager {
private:
    Context& ctx;
    std::vector<DescriptorEntry> entries;
    vk::raii::DescriptorPool pool{nullptr};
    
public:
    DescriptorManager(Context& ctx) : ctx(ctx) {}

    DescriptorEntry new_entry();
};

}