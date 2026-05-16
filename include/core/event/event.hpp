#pragma once

#include "core/utils/ecs/system.hpp"
#include <functional>

namespace bottle::core::event {

class Event {
};

class EventSystem : public utils::System {
private:
    std::function<void(Event)> callback;
};

}