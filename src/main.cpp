#include "core/render/renderComponent.hpp"
#include "core/render/uniform.hpp"
#include "core/utils/ecs/entity.hpp"
#include "core/utils/locator.hpp"
#include <meshes.hpp>
#include <core/render/renderSystem.hpp>
#include <bottle.hpp>
#include <core/resources/render/shader.hpp>

using namespace bottle;
using Entity = utils::Entity;

class RenderEntity : public Entity {
private:
    float t;

public:
    void stage0() {
        utils::Locator::Instance().get<core::render::RenderSystem>()->getUniform().addType("time", core::render::Uniform::UniformType::FLOAT);
    }

    void stage1() {
        this->addComponent(new core::render::RenderComponent(
        bottle::core::render::Mesh {
            std::vector<bottle::core::render::Vertex>{
                core::render::Vertex{-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0},
                core::render::Vertex{0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0},
                core::render::Vertex{0.5f, 0.5f, 0.0f, 0.0, 1.0, 0.0},
                core::render::Vertex{-0.5f, 0.5f, 0.0f, 0.0, 1.0, 0.0},
            },
            std::vector<uint32_t>{ 0, 1, 2, 0, 2, 3}
        },
        std::vector<bottle::core::resources::render::ShaderResource*>{
            (new bottle::core::resources::render::Shader("shaders/frag.frag.spv", bottle::core::resources::render::Shader::TYPE::FRAGMENT))->getResource(),
            (new bottle::core::resources::render::Shader("shaders/vert.vert.spv", bottle::core::resources::render::Shader::TYPE::VERTEX))->getResource()
        }
        ));
    }

    RenderEntity() {
        initStages.emplace_back([this]() { stage0(); });
        initStages.emplace_back([this]() { stage1(); });
    }

    void update() override {
        utils::Locator::Instance().get<core::render::RenderSystem>()->getUniform().set("time", t);
        t += 0.001;
    }
};

int main() {
    Engine bottle;

    RenderEntity* entity = new RenderEntity{};
    bottle.addEntity("entity", entity);

    bottle.init();
    bottle.run();
}