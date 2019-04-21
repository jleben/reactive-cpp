#pragma once

#include "event.h"
#include "object.h"
#include "signal.h"

#include <thread>
#include <functional>
#include <vector>

namespace Reactive {

using std::thread;
using std::vector;

class Event;
class Object;

class Reaction
{
public:
    using Function = std::function<void()>;

    Reaction(const Event &,
             const vector<Object*> & used_objects,
             const vector<Object*> & modified_objects,
             Function function);
    ~Reaction();

private:
    void work();
    void do_function();
    void lock_objects();
    void unlock_objects();

    Event d_event;
    std::function<void()> d_func;
    thread d_thread;
    Signal d_signal;

    struct Used_Object
    {
        Object * p = nullptr;
        bool modified = false;
    };

    vector<Used_Object> d_objects;
};

inline
Reaction::~Reaction()
{
    d_signal.notify();
    d_thread.join();
}

}
