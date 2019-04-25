#pragma once

#include "event.h"
#include "object.h"
#include "signal.h"

#include <thread>
#include <functional>
#include <vector>
#include <memory>

namespace Reactive {

using std::thread;
using std::vector;

class Event;
class Object;

using Function = std::function<void()>;

struct Reaction_Fragment
{
    Reaction_Fragment(Function f): function(f) {}

    Function function;
    vector<Object*> used_objects;
    vector<Object*> modified_objects;

    Reaction_Fragment & Using(Object* object)
    {
        used_objects.push_back(object);
        return *this;
    }
    Reaction_Fragment & Modifying(Object* object)
    {
        modified_objects.push_back(object);
        return *this;
    }
};

struct Reaction
{
    Reaction() {}
    Reaction(const Reaction_Fragment & f): sequence({f}) {}
    vector<Reaction_Fragment> sequence;
};

inline
Reaction_Fragment Do(Function f)
{
    return Reaction_Fragment { f };
}

inline
Reaction operator+(const Reaction_Fragment & a, const Reaction_Fragment & b)
{
    Reaction r;
    r.sequence = { a, b };
    return r;
}

inline
Reaction operator+(const Reaction & r, const Reaction_Fragment & f)
{
    Reaction r2 = r;
    r2.sequence.push_back(f);
    return r2;
}

class Reactor
{
public:
    using Function = std::function<void()>;

    Reactor(const Event &,
            const Reaction &);
    ~Reactor();

    Reactor() {}
    Reactor(const Reactor & other) = delete;
    Reactor & operator=(const Reactor & other) = delete;

    Reactor(Reactor && other) = default;
    Reactor & operator= (Reactor && other) = default;

private:

    struct Used_Object
    {
        Object * p = nullptr;
        bool modified = false;
    };

    class Fragment
    {
    public:
        Fragment() {}
        Fragment(const Reaction_Fragment &);
        void operator()();

    private:
        void lock_objects();
        void unlock_objects();

        std::function<void()> func;
        vector<Used_Object> objects;
    };

    struct Implementation
    {
        void work();
        void react();

        Event event;
        std::thread thread;
        Signal stop_signal;

        vector<Fragment> fragments;
    };

    Implementation * d = nullptr;
};

inline
Reactor operator >> (const Event & e, const Reaction & r)
{
    return Reactor(e, r);
}

inline
Reactor operator >> (const Event & e, Function f)
{
    return Reactor(e, Reaction(f));
}

}
