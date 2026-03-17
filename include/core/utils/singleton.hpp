#pragma once

template<typename T>
class Singleton {
public:
    static T& Instance() {
        static T s;
        return s;
    }

    Singleton(const Singleton& s) = delete;
    T operator=(const T& s) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};