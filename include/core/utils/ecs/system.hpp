#pragma once

namespace bottle::utils {

class System {
public:
    virtual void update() = 0;
    virtual ~System() {}
};

}