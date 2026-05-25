#pragma once

#include <vector>
#include <functional>

namespace bottle::core::utils {

class System {
protected:
    std::vector<std::function<void()>> initStages;
    std::vector<std::function<void()>> destroyStages;

public:
    std::vector<std::function<void()>> getInitStages() { return initStages; }
    std::vector<std::function<void()>> getDestroyStages() { return destroyStages; }
    virtual void update() = 0;
    virtual ~System() {}
};

}