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

    Reaction() {}
    Reaction(const Reaction & other) = delete;
    Reaction & operator=(const Reaction & other) = delete;

    Reaction(Reaction && other) = default;
    Reaction & operator= (Reaction && other) = default;

private:

    struct Used_Object
    {
        Object * p = nullptr;
        bool modified = false;
    };

    struct Implementation
    {
        void work();
        void do_function();
        void lock_objects();
        void unlock_objects();

        Event event;
        std::function<void()> func;
        std::thread thread;
        Signal stop_signal;

        vector<Used_Object> objects;
    };

    Implementation * d = nullptr;
};

}
