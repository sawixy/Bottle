#pragma once

namespace bottle::utils {

class System {
public:
    virtual void pastInit() {};
    virtual void update() = 0;
    virtual ~System() {}
};

}