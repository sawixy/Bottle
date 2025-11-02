#include <System.hpp>
#include <vector>
#include <string>
#include <memory>
#include "src/pipeline/pipelinecreate.hpp"
#include "src/render.hpp"
#include "src/pipeline/shader.hpp"
#include <tuple>
#include <unordered_map>

class RenderComponent {
private:
    Render* _render;
    PipelineCreate* pipelineCreate;
    std::vector<Shader> shaders;

public:
    RenderComponent(Render* render);
    ~RenderComponent();
};

class RenderSystem : public System<RenderComponent> {
private:
    std::vector<RenderComponent*> components;
public:
    void init();
    void update();
    void add(RenderComponent* component);
};