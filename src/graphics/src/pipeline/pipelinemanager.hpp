#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <stdexcept>
#include "shader.hpp"

// forward declaration
class App;

class PipelineManager {
private:
    std::shared_ptr<App> app;
    std::unordered_map<std::string, VkPipeline> pipelines;
    VkPipelineCache pipelineCache;

public:
    PipelineManager(std::shared_ptr<App> app){
        app = app;
    }

    ~PipelineManager(){
        
    }
};