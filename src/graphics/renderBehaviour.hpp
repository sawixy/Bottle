#include <System.hpp>
#include <vector>
#include <string>
#include <memory>
#include "src/pipeline/pipelinecreate.hpp"
#include "src/render.hpp"
#include "src/pipeline/shader.hpp"
#include <tuple>
#include <unordered_map>

class RenderPart {
private:
    Render* _render;
    PipelineCreate* pipelineCreate;
    std::vector<Shader> shaders;

public:
    RenderPart(Render* render);
    ~RenderPart();
};

class RenderBehaviour : public System<RenderPart> {
private:
    std::vector<RenderPart*> components;
public:
    void init();
    void update();
    void add(RenderPart* component);
};