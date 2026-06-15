#pragma once

#include <core/utils/locator.hpp>
#include <core/resources/render/shaderResource.hpp>
#include <core/utils/ecs/system.hpp>
#include <stdint.h>
#include <core/render/renderComponent.hpp>
#include <core/render/uniform.hpp>

namespace bottle::core::render {

class RenderSystem: public utils::System {
public:
    virtual void addComponent(RenderComponent* comp) = 0;
    virtual void update() override = 0;
    virtual ~RenderSystem() override = default;

    virtual Uniform& getUniform(int binding) = 0;
};

}