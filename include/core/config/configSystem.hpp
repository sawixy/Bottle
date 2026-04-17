#pragma once

#include <unordered_map>
#include <string>

#include <core/utils/singleton.hpp>

namespace bottle::core::config {

class ConfigSystem : public Singleton<ConfigSystem> {
private:
    std::unordered_map<std::string, std::string> config;
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;

};

}