#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bottle::core::transform {

class TransformComponent {
private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

public:
    TransformComponent(glm::vec3 pos, glm::quat rot, glm::vec3 scal) : position(pos), rotation(rot), scale(scal) {}
};

}