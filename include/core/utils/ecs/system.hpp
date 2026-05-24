#pragma once

#include <vector>
#include <functional>

namespace bottle::core::utils {

class System {
protected:
    std::vector<std::function<void()>> initStages;

public:
    std::vector<std::function<void()>> getInitStages() { return initStages; }
    virtual void update() = 0;
    virtual ~System() {}
};

}