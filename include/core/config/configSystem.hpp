#pragma once

#include <core/utils/ecs/system.hpp>
#include <string>
#include <any>
#include <core/utils/singleton.hpp>

namespace bottle::core::config {

class ConfigSystem : public utils::System {
public:
    ConfigSystem() = default;
    virtual ~ConfigSystem() = default;

    virtual void load(const std::string& filename) = 0;
    virtual std::any getAny(const std::string& key) = 0;
    virtual void setAny(const std::string& key, const std::any& value) = 0;
    
    template<typename T>
    T get(const std::string& key) {
        return std::any_cast<T>(getAny(key));
    }
    
    template<typename T>
    void set(const std::string& key, const T& value) {
        setAny(key, std::any(value));
    }
    
    void update() override {};
};

}