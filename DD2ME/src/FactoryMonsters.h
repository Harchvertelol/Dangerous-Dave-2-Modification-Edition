#ifndef FACTORYMONSTERS_H
#define FACTORYMONSTERS_H
#include <iostream>
#include <map>
#include <vector>
#include "CreatureMonster.h"

class Game;

class FactoryMonsters
{
    public:
        FactoryMonsters(Game*);
        ~FactoryMonsters();
        Game* s_GameClass;
        std::map<int, CreatureMonster*> s_Monsters;
        std::vector<CreatureMonster*> s_QueueForAddingMonsters;
        std::map<std::string, std::string> s_AIMonstersValues;
        int s_MaxIndex;
        CreatureMonster* addMonster(int, int, int, bool getstate = true);
        int addMonsterImmediately(int, int, int, bool getstate = true);
        void removeMonster(int);
        void addMonstersFromQueue();
        void live();
        void clear();
        void reloadAIAll();
        void drawAll();
};
#endif
