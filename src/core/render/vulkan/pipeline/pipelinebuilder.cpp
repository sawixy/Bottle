#include "core/resources/render/shaderResource.hpp"
#include <core/render/vulkan/pipeline/pipelinebuilder.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace bottle::core::render::vulkan {

PipelineWithShaders PipelineBuilder::build() {
    std::vector<vk::raii::ShaderModule> modules;

    for (auto shader : shaders) {
        // TODO: Make exception checking
        modules.push_back(std::get<resources::render::VulkanCode>(shader->getCode().code));
    }

    vk::GraphicsPipelineCreateInfo pipelineCI {
        {},
        static_cast<uint32_t>(shaders.size()),
        modules.data(),

    };
}

}