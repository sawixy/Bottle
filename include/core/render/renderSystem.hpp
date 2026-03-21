#pragma once

#include <core/utils/ecs/system.hpp>

namespace bottle::core::render {

class RenderSystem: public utils::System {
public:
    virtual void update() override = 0;
    virtual ~RenderSystem() override = 0;
};

}