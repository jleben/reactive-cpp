#include "reaction.h"

#include <unistd.h>
#include <poll.h>

namespace Reactive {

Reaction::Reaction(const Event & event,
                   const vector<Object*> & used_objects,
                   const vector<Object*> & modified_objects,
                   Function function):
    d_event(event),
    d_func(function)
{
    for(auto * object : used_objects)
    {
        bool modified = false;
        d_objects.push_back({ object, modified });
    }

    for(auto * object : modified_objects)
    {
        bool modified = true;
        d_objects.push_back({ object, modified });
    }

    std::sort(d_objects.begin(), d_objects.end(),
              [](const Used_Object & a, const Used_Object & b){ return a.p < b.p; });

    d_thread = thread(&Reaction::work, this);
}

void Reaction::work()
{
    pollfd data[2];

    data[0].fd = d_event.fd;
    data[0].events = d_event.poll_events;

    {
        auto sig_event = d_signal.event();
        data[1].fd = sig_event.fd;
        data[1].events = sig_event.poll_events;
    }

    int result;

    while(true)
    {
        result = poll(data, 2, -1);

        if (result == -1 && errno != EINTR)
            break;

        if (data[1].revents)
            break;

        if (data[0].revents)
        {
            do_function();
            d_event.clear();
        }
    }

    if (result == -1)
        throw std::runtime_error("'poll' failed.");
}

void Reaction::do_function()
{
    lock_objects();

    try
    {
        d_func();
    }
    catch(...)
    {
        unlock_objects();
        throw;
    }

    unlock_objects();
}

void Reaction::lock_objects()
{
    for (auto & object : d_objects)
    {
        if (object.modified)
            object.p->d_mutex.lock();
        else
            object.p->d_mutex.lock_shared();
    }
}

void Reaction::unlock_objects()
{
    for (auto & object : d_objects)
    {
        object.p->d_mutex.unlock();
    }
}

}
