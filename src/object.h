#pragma once

#include <shared_mutex>
#include <unordered_set>

namespace Reactive {

class Reactor;

class Object
{
public:
    bool can_use() const;
    bool can_modify() const;

private:
    friend class Reactor;
    std::shared_mutex d_mutex;

    void lock_use()
    {
        d_mutex.lock_shared();
        d_can_use.insert(this);
    }

    void lock_modify()
    {
        d_mutex.lock();
        d_can_modify.insert(this);
    }

    void unlock()
    {
        d_mutex.unlock();
        d_can_use.erase(this);
        d_can_modify.erase(this);
    }

    thread_local static std::unordered_set<const Object*> d_can_use;
    thread_local static std::unordered_set<const Object*> d_can_modify;
};

template <typename T>
class Safe : public Object
{
public:
    const T & use()
    {
        if (!can_use())
            throw std::runtime_error("Can not use.");
        return d;
    }

    T & mod()
    {
        if (!can_modify())
            throw std::runtime_error("Can not modify.");
        return d;
    }

private:
    T d;
};


}
