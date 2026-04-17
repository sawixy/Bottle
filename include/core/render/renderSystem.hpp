#pragma once

#include <core/utils/locator.hpp>
#include <core/resources/render/shaderResource.hpp>
#include <core/utils/ecs/system.hpp>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>
#include <core/render/renderComponent.hpp>

namespace bottle::core::render {

class RenderSystem: public utils::System {
public:
    virtual void addComponent(RenderComponent* comp) = 0;
    virtual void update() override = 0;
    virtual ~RenderSystem() override = default;
};

}