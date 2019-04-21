#pragma once

#include <cstdint>
#include <functional>

namespace Reactive {

using std::uint32_t;
using std::function;

class Event
{
public:
    int fd;
    uint32_t epoll_events;
    short poll_events;
    function<void()> clear;
};

}
