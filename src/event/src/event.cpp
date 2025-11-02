#include "event.hpp"

void Event::add(std::function<void()> callback) {
    callbacks.push_back(callback);
}

void Event::emit() {
    for (auto func : callbacks){
        func();
    }
}