#include "../src/reaction.h"

#include <thread>
#include <chrono>

using namespace std;
using namespace Reactive;

int main()
{
    Signal s1;
    Signal s2;

    Safe<int> o1;

    Reaction r1(s1.event(), {&o1}, {}, []()
    {
        printf("*\n");
    });

    Reaction r2(s2.event(), {&o1}, {}, [&o1]()
    {
        printf(">--- %p\n", &o1.use());
        this_thread::sleep_for(chrono::milliseconds(500));
        printf("<---\n");
    });

    int count = 10;
    while(count--)
    {
        s2.notify();
        this_thread::sleep_for(chrono::milliseconds(200));
        s1.notify();
        this_thread::sleep_for(chrono::milliseconds(800));
    }
}
