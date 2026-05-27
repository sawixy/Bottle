#include "core/render/renderComponent.hpp"
#include "core/render/uniform.hpp"
#include "core/utils/ecs/component.hpp"
#include "core/utils/ecs/entity.hpp"
#include "core/utils/locator.hpp"
#include <glm/ext/matrix_float4x4.hpp>
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
                {"color", core::render::Uniform::UniformType::VEC3}
             }
        );

        this->addComponent(renderComponent);
    }

    RenderEntity() {
        initStages.emplace_back([this]() { stage0(); });
        initStages.emplace_back([this]() { stage1(); });
    }

    void update() override {
        static float time;
        this->getComponent<core::render::RenderComponent>()->getUniform().set("color", glm::vec3(sin(time), cos(time), sin(cos(time))));
        time += 0.001;
    }
};

int main() {
    Engine bottle;

    RenderEntity* entity = new RenderEntity{};
    bottle.addEntity("test", entity);

    bottle.init();
    bottle.run();
}