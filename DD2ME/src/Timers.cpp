#include "Timers.h"

using namespace std;

using namespace sf;

Timers::Timers()
{
    //...
}

Timers::~Timers()
{
    //...
}

int Timers::setTimer(int ms)
{
    TimerInfo adding_ti;
    adding_ti.s_TimerTick = ms;
    adding_ti.s_Timer.restart();
    s_Timers.push_back(adding_ti);
    return s_Timers.size() - 1;
}

vector<int> Timers::getTimersTicks()
{
    vector<int> ret_vect;
    for(int i = 0; i < s_Timers.size(); i++)
    {
        if(s_Timers[i].s_Timer.getElapsedTime().asMilliseconds() > s_Timers[i].s_TimerTick)
        {
            ret_vect.push_back(i);
            s_Timers[i].s_Timer.restart();
        }
    }
    return ret_vect;
}
