#pragma once

#include <core/config/configSystem.hpp>
#include <rapidjson/document.h>
#include <string>

namespace bottle::core::config {

class JSONConfigSystem : public ConfigSystem {
private:
    rapidjson::Document config;
public:
    void load(const std::string& filename) override;
    std::any getAny(const std::string& key) override;
    void setAny(const std::string& key, const std::any& value) override;
};

}