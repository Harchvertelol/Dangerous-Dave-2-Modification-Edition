#ifndef FACTORYMONSTERS_H
#define FACTORYMONSTERS_H
#include <iostream>
#include <map>
#include "CreatureMonster.h"

class Game;

class FactoryMonsters
{
    public:
        FactoryMonsters(Game*);
        ~FactoryMonsters();
        Game* s_GameClass;
        std::map<int, CreatureMonster*> s_Monsters;
        int s_MaxIndex;
        int addMonster(int,int,int,bool getstate = true);
        void removeMonster(int);
        void live();
        void clear();
        void reloadAIAll();
        void drawAll();
};
#endif
