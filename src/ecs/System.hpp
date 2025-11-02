template<typename T>
class System {
    virtual void init();
    virtual void update();
    virtual void add(T* component);
};