#pragma once

#include <core/utils/ecs/system.hpp>
#include <unordered_map>
#include <string>

#include <core/utils/singleton.hpp>

namespace bottle::core::config {

class ConfigSystem : public utils::System {
protected:
    std::unordered_map<std::string, std::string> config;

    void load_json(std::string filename);

public:
    ConfigSystem() = default;

    void load(std::string filename);
    void set(const std::string& key, const std::string& value) { config[key] = value; }
    std::string& get(const std::string& key) { return config[key]; };
    void update() override {};
};

}