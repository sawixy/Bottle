#pragma once

#include <functional>

class Event{
private:
    std::vector<std::function<void()>> callbacks{};

public:
    void add(std::function<void()> callback);
    void emit();
};