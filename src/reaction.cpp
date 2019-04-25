#include "reaction.h"

#include <unistd.h>
#include <poll.h>

namespace Reactive {

Reactor::Reactor(const Event & event, const Reaction & reaction)
{
    d = new Implementation;
    d->event = event;

    for (auto & fragment : reaction.sequence)
    {
        d->fragments.emplace_back(fragment);
    }

    d->thread = std::thread(&Reactor::Implementation::work, d);
}

Reactor::~Reactor()
{
    if (d && d->thread.joinable())
    {
        d->stop_signal.notify();
        d->thread.join();
    }

    delete d;
}

void Reactor::Implementation::work()
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
            react();
            event.clear();
        }
    }

    if (result == -1)
        throw std::runtime_error("'poll' failed.");
}

void Reactor::Implementation::react()
{
    for (auto & fragment : fragments)
    {
        fragment();
    }
}

Reactor::Fragment::Fragment(const Reaction_Fragment & reaction):
    func(reaction.function)
{
    for(auto * object : reaction.used_objects)
    {
        bool modified = false;
        objects.push_back({ object, modified });
    }

    for(auto * object : reaction.modified_objects)
    {
        bool modified = true;
        objects.push_back({ object, modified });
    }

    std::sort(objects.begin(), objects.end(),
              [](const Used_Object & a, const Used_Object & b){ return a.p < b.p; });
}

void Reactor::Fragment::operator()()
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

void Reactor::Fragment::lock_objects()
{
    for (auto & object : objects)
    {
        if (object.modified)
            object.p->lock_modify();
        else
            object.p->lock_use();
    }
}

void Reactor::Fragment::unlock_objects()
{
    for (auto & object : objects)
    {
        object.p->unlock();
    }
}

}
