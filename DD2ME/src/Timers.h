#ifndef TIMERS_H
#define TIMERS_H
#include <SFML/System.hpp>

struct TimerInfo
{
    sf::Clock s_Timer;
    int s_TimerTick = -1;
};

class Timers
{
    public:
        Timers();
        ~Timers();
        std::vector<TimerInfo> s_Timers;
        int setTimer(int);
        std::vector<int> getTimersTicks();
};

#endif
