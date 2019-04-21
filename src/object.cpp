#include "object.h"

namespace Reactive {

thread_local std::unordered_set<const Object*> Object::d_can_use;
thread_local std::unordered_set<const Object*> Object::d_can_modify;

bool Object::can_use() const
{
    return d_can_use.count(this) || d_can_modify.count(this);
}

bool Object::can_modify() const
{
    return d_can_modify.count(this);
}

}
