#ifndef FACTORYMONSTERS_H
#define FACTORYMONSTERS_H
#include <iostream>
#include <map>
#include "CreatureMonster.h"
using namespace std;

class Game;

class FactoryMonsters
{
    public:
        FactoryMonsters(Game*);
        ~FactoryMonsters();
        Game* s_GameClass;
        map<int,CreatureMonster*> s_Monsters;
        int s_MaxIndex;
        int addMonster(int,int,int,bool getstate = true);
        void removeMonster(int);
        void live();
        void clear();
        void drawAll();
};
#endif
