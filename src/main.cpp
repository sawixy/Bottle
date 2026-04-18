#include "core/render/renderComponent.hpp"
#include "core/utils/ecs/entity.hpp"
#include <meshes.hpp>
#include <core/render/renderSystem.hpp>
#include <bottle.hpp>
#include <core/resources/render/shader.hpp>

using namespace bottle;
using Entity = utils::Entity;

int main() {
    Engine bottle;
    bottle.init();

    Entity entity;
    entity.addComponent(new core::render::RenderComponent( 
        bottle::core::render::Mesh {
            std::vector<bottle::core::render::Vertex>{
                core::render::Vertex{-1.0f, -1.0f, 0.0f, 1.0, 0.0, 0.0},
                core::render::Vertex{1.0f, -1.0f, 0.0f, 1.0, 0.0, 0.0},
                core::render::Vertex{1.0f, 1.0f, 0.0f, 0.0, 1.0, 0.0},
                core::render::Vertex{-1.0f, 1.0f, 0.0f, 0.0, 1.0, 0.0},
            },
            std::vector<uint32_t>{ 0, 1, 2, 0, 2, 3}
        },
        std::vector<bottle::core::resources::render::ShaderResource*>{
            (new bottle::core::resources::render::Shader("shaders/frag.frag.spv", bottle::core::resources::render::Shader::TYPE::FRAGMENT))->getResource(),
            (new bottle::core::resources::render::Shader("shaders/vert.vert.spv", bottle::core::resources::render::Shader::TYPE::VERTEX))->getResource()
        }
    ));
    bottle.addEntity("entity", entity);


    bottle.run();
}