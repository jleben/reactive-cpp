#pragma once

#include <shared_mutex>

namespace Reactive {

class Reaction;

class Object
{
private:
    friend class Reaction;
    std::shared_mutex d_mutex;
};

}
