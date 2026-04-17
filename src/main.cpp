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
    entity.addComponent(new core::render::RenderComponent(bottle::createTriangleMesh(), std::vector<bottle::core::resources::render::ShaderResource*>{
        (new bottle::core::resources::render::Shader("shaders/triangle.frag.spv", bottle::core::resources::render::Shader::TYPE::FRAGMENT))->getResource(),
        (new bottle::core::resources::render::Shader("shaders/triangle.vert.spv", bottle::core::resources::render::Shader::TYPE::VERTEX))->getResource()
    }));
    bottle.addEntity("entity", entity);

    Entity entity2;
    entity2.addComponent(new core::render::RenderComponent(bottle::createTriangleMesh(), std::vector<bottle::core::resources::render::ShaderResource*>{
        (new bottle::core::resources::render::Shader("shaders/triangle.frag.spv", bottle::core::resources::render::Shader::TYPE::FRAGMENT))->getResource(),
        (new bottle::core::resources::render::Shader("shaders/triangle2.vert.spv", bottle::core::resources::render::Shader::TYPE::VERTEX))->getResource()
    }));
    bottle.addEntity("entity2", entity2);


    bottle.run();
}