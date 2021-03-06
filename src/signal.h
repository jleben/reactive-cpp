#pragma once

#include "event.h"

namespace Reactive {

/*!
  \brief Notifies other threads using an Event.

  This class lets one or more threads wait for
  an Event which is activated from another thread.

  Note that only one Event is provided, so
  it can be handled by multiple threads only
  if they are all waiting for it to be activated
  at the same time.

  \sa
  \ref Signal_Sender for notifying multiple threads
  independently (though less efficiently).
*/
class Signal
{

public:
    Signal();
    ~Signal();

    /*! \brief Activate the \ref event. */
    void notify();

    /*! \brief Returns a momentary event activated when \ref notify is called. */
    Event event();

    Signal(const Signal & other) = delete;
    Signal & operator=(const Signal & other) = delete;

    Signal(Signal && other): d_fd(other.d_fd) {}
    Signal & operator=(Signal && other) { d_fd = other.d_fd; return *this; }

private:
    static void clear(int fd);

    int d_fd;
};

}

