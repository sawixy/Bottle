#include "core/render/renderComponent.hpp"
#include "core/render/uniform.hpp"
#include "core/transform.hpp"
#include "core/utils/ecs/component.hpp"
#include "core/utils/ecs/entity.hpp"
#include "core/utils/locator.hpp"
#include <core/transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <meshes.hpp>
#include <core/render/renderSystem.hpp>
#include <bottle.hpp>
#include <core/resources/render/shader.hpp>

using namespace bottle;
using Entity = core::utils::Entity;

class RenderEntity : public Entity {
public:
    void stage0() {
    }

    void stage1() {
        core::render::RenderComponent* renderComponent = new core::render::RenderComponent(
            bottle::core::render::Mesh {
                std::vector<bottle::core::render::Vertex>{
                    core::render::Vertex{-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f},
                    core::render::Vertex{ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f},
                    core::render::Vertex{ 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f},
                    core::render::Vertex{-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f},
                    
                    core::render::Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
                    core::render::Vertex{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
                    core::render::Vertex{ 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f},
                    core::render::Vertex{-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f},
                },
                std::vector<uint32_t>{
                    0, 1, 2, 0, 2, 3,
                    4, 5, 7, 5, 6, 7,
                    1, 5, 6, 1, 6, 2,
                    0, 4, 7, 0, 7, 3,
                    3, 2, 6, 3, 6, 7,
                    0, 1, 5, 0, 5, 4
                }
            },
            std::vector<bottle::core::resources::render::ShaderResource*>{
                bottle::core::resources::render::Shader("shaders/frag.frag.spv", bottle::core::resources::render::Shader::TYPE::FRAGMENT).getResource(),
                bottle::core::resources::render::Shader("shaders/vert.vert.spv", bottle::core::resources::render::Shader::TYPE::VERTEX).getResource()
            },
            std::unordered_map<std::string, core::render::Uniform::UniformType>{
                {"model", core::render::Uniform::UniformType::MAT4},
                {"view", core::render::Uniform::UniformType::MAT4},
                {"proj", core::render::Uniform::UniformType::MAT4}
             }
        );

        core::transform::TransformComponent* transformComponent = new core::transform::TransformComponent(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        this->addComponent(renderComponent);
        this->addComponent(transformComponent);
    }

    RenderEntity() {
        initStages.emplace_back([this]() { stage0(); });
        initStages.emplace_back([this]() { stage1(); });
    }

    void update() override {
        this->getComponent<core::render::RenderComponent>()->getUniform().set("model", this->getComponent<core::transform::TransformComponent>()->getModelMatrix());
        this->getComponent<core::render::RenderComponent>()->getUniform().set("view", glm::lookAt(glm::vec3(0.0, 0.0, -2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
        this->getComponent<core::render::RenderComponent>()->getUniform().set("proj", glm::perspective(glm::radians(70.0f), 800.0f / 600.0f, 0.1f, 100.0f));
    }
};

int main() {
    Engine bottle;

    RenderEntity* entity = new RenderEntity{};
    bottle.addEntity("test", entity);

    bottle.init();
    bottle.run();
}