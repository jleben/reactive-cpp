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

    printf("Hi.\n");

    Reactor r1(s1.event(), Do([](){
        printf("*\n");
    }));

    Reactor r2(s2.event(),
    Do([&o1]()
    {
        printf(">--- %p\n", &o1.use());
        this_thread::sleep_for(chrono::milliseconds(100));
        printf("<---\n");
    })
    .Modifying(&o1)
    &&
    Do([]{
        this_thread::sleep_for(chrono::milliseconds(300));
        printf("Yas!\n");
    }).Modifying(&o1));

    int count = 10;
    while(count--)
    {
        s2.notify();
        this_thread::sleep_for(chrono::milliseconds(200));
        s1.notify();
        this_thread::sleep_for(chrono::milliseconds(800));
    }
}
