#pragma once

#include "core/utils/ecs/system.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace bottle::core::event {

class EventSystem : public utils::System {
private:
    using Callback = std::function<void()>;
    std::unordered_map<std::string, std::vector<Callback>> callbacks;

public:
    EventSystem() = default;

    void registerEvent(const std::string& eventName) {
        if (callbacks.find(eventName) == callbacks.end()) {
            callbacks[eventName] = {};
        }
    }

    void subscribe(const std::string& eventName, Callback callback) {
        callbacks[eventName].push_back(std::move(callback));
    }

    void emit(const std::string& eventName) {
        auto it = callbacks.find(eventName);
        if (it != callbacks.end()) {
            for (auto& callback : it->second) {
                callback();
            }
        }
    }

    void update() override {}
};

}