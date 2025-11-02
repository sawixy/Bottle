#include "renderSystem.hpp"

RenderComponent::RenderComponent(Render* render)  {
    _render = render;
    shaders.push_back(Shader(render, "shaders/vert.spv", ShaderType::VERTEX));
    shaders.push_back(Shader(render, "shaders/frag.spv", ShaderType::FRAGMENT));
    pipelineCreate = new PipelineCreate(render, shaders, render->renderpass);
}

RenderComponent::~RenderComponent() {
    if (pipelineCreate) {
        delete pipelineCreate;
    }
}