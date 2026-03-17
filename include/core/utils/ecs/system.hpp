#pragma once

namespace bottle::utils {

class System {
public:
    virtual void update();
    virtual ~System() = 0;
};

}