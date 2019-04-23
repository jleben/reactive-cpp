#include "reaction.h"

#include <unistd.h>
#include <poll.h>

namespace Reactive {

Reaction::Reaction(const Event & event,
                   const vector<Object*> & used_objects,
                   const vector<Object*> & modified_objects,
                   Function function)
{
    d = new Implementation;
    d->event = event;
    d->func = function;

    for(auto * object : used_objects)
    {
        bool modified = false;
        d->objects.push_back({ object, modified });
    }

    for(auto * object : modified_objects)
    {
        bool modified = true;
        d->objects.push_back({ object, modified });
    }

    std::sort(d->objects.begin(), d->objects.end(),
              [](const Used_Object & a, const Used_Object & b){ return a.p < b.p; });

    d->thread = std::thread(&Reaction::Implementation::work, d);
}

Reaction::~Reaction()
{
    if (d && d->thread.joinable())
    {
        d->stop_signal.notify();
        d->thread.join();
    }

    delete d;
}

void Reaction::Implementation::work()
{
    pollfd data[2];

    data[0].fd = event.fd;
    data[0].events = event.poll_events;

    {
        auto sig_event = stop_signal.event();
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
            event.clear();
        }
    }

    if (result == -1)
        throw std::runtime_error("'poll' failed.");
}

void Reaction::Implementation::do_function()
{
    lock_objects();

    try
    {
        func();
    }
    catch(...)
    {
        unlock_objects();
        throw;
    }

    unlock_objects();
}

void Reaction::Implementation::lock_objects()
{
    for (auto & object : objects)
    {
        if (object.modified)
            object.p->lock_modify();
        else
            object.p->lock_use();
    }
}

void Reaction::Implementation::unlock_objects()
{
    for (auto & object : objects)
    {
        object.p->unlock();
    }
}

}
