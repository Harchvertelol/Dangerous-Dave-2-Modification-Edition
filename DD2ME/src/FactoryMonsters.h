#ifndef FACTORYMONSTERS_H
#define FACTORYMONSTERS_H
#include <iostream>
#include <map>
#include <vector>
#include "CreatureMonster.h"
#include "CreaturePlayer.h"

class Game;

enum MONSTER_PLAYER_RADIUS_TYPE
{
    MDRT_LIVE = 0,
    MDRT_ACTIVATE
};

class FactoryMonsters
{
    public:
        FactoryMonsters(Game*);
        ~FactoryMonsters();
        Game* s_GameClass;
        std::map<int, CreatureMonster*> s_Monsters;
        std::vector<CreatureMonster*> s_QueueForAddingMonsters;
        //std::map<std::string, std::string> s_AIMonstersValues;
        IniParser::PostParsingStruct* s_AIMonstersValues;
        unsigned int s_MaxIndex;
        CreatureMonster* addMonster(int, int, int, bool getstate = true);
        int addMonsterImmediately(int, int, int, bool getstate = true);
        bool isMonsterInPlayerRadius(CreaturePlayer*, CreatureMonster*, MONSTER_PLAYER_RADIUS_TYPE);
        void removeMonster(int);
        void addMonstersFromQueue();
        bool isExistsById(unsigned int);
        CreatureMonster* getMonsterById(int id);
        void live();
        void clear();
        void reloadAIAll();
        void drawAll();
};
#endif
