#pragma once

#include "core/utils/ecs/component.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bottle::core::transform {

class TransformComponent : public utils::Component {
private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

public:
    TransformComponent(glm::vec3 pos, glm::quat rot, glm::vec3 scal) : position(pos), rotation(rot), scale(scal) {}

    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }

    void rotate(glm::quat delta) { rotation = delta * rotation; }
    void translate(glm::vec3 delta) { position += delta; }
    void setScale(glm::vec3 newScale) { scale = newScale; }

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model *= glm::translate(model, position);
        model *= glm::mat4_cast(rotation);
        model *= glm::scale(model, scale);
        return model;
    }
};

}