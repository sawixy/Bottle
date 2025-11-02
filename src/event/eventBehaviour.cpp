#include "eventsystem.hpp"

Event EventBehaviour::register_event(EventTrigger trigger) {
    Event event{};
    events[trigger] = event;

    return event;
}