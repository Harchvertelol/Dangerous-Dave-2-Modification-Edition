#include "FactoryMonsters.h"

#include "Game.h"

#include "WorkFunctions.h"
#include "Defines.h"

#include "Thread/Thread.h"

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

using namespace std;

FactoryMonsters::FactoryMonsters(Game* gameclass):
    s_GameClass(gameclass),
    s_MaxIndex(0)
{
    s_AIMonstersValues = new PostParsingStruct;
}

FactoryMonsters::~FactoryMonsters()
{
    if(s_AIMonstersValues != 0) delete s_AIMonstersValues;
    clear();
}

void FactoryMonsters::removeMonster(int key)
{
    delete s_Monsters[key];
    s_Monsters.erase(key);
}

int FactoryMonsters::addMonsterImmediately(int number, int x, int y, bool getstate)
{
    s_Monsters[s_MaxIndex] = new CreatureMonster(s_GameClass, number, s_MaxIndex, x, y, getstate);
    s_MaxIndex++;
    return s_MaxIndex - 1;
}

bool FactoryMonsters::isExistsById(unsigned int id_mnst)
{
    map<int, CreatureMonster*>::iterator I = s_Monsters.find(id_mnst);
    if ( I == s_Monsters.end() ) return false;
    return true;
}

CreatureMonster* FactoryMonsters::addMonster(int number, int x, int y, bool getstate)
{
    CreatureMonster* newMonster = new CreatureMonster(s_GameClass, number, s_MaxIndex + s_QueueForAddingMonsters.size(), x, y, getstate);
    s_QueueForAddingMonsters.push_back(newMonster);
    return newMonster;
}

void FactoryMonsters::addMonstersFromQueue()
{
    if(s_QueueForAddingMonsters.empty()) return;
    for(unsigned int i = 0; i < s_QueueForAddingMonsters.size(); i++)
    {
        s_Monsters[s_MaxIndex] = s_QueueForAddingMonsters[i];
        //s_Monsters[s_MaxIndex]->s_ID = s_MaxIndex;
        if(s_MaxIndex != s_QueueForAddingMonsters[i]->s_ID) cout << "Error with monster ID!" << endl;
        s_MaxIndex++;
    }
    s_QueueForAddingMonsters.clear();
}

bool FactoryMonsters::isMonsterInPlayerRadius(CreaturePlayer* player, CreatureMonster* mnst, MONSTER_PLAYER_RADIUS_TYPE rad_type)
{
    int radiusX = s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX, radiusY = s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY;
    /*if(rad_type == MDRT_LIVE)
    {
        radiusX = s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX;
        radiusY = s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY;
    }*/
    if(rad_type == MDRT_ACTIVATE)
    {
        radiusX = s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX;
        radiusX = s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY;
    }
    /*if(mnst->s_CoordX <= s_GameClass->s_DisplayStruct->s_GameResolutionX + player->s_ScreenCoordX + 16 * radiusX &&
        mnst->s_CoordX >= player->s_ScreenCoordX - 16 * radiusX &&
        mnst->s_CoordY <= s_GameClass->s_DisplayStruct->s_GameResolutionY + player->s_ScreenCoordY + 16 * radiusY &&
        mnst->s_CoordY >= player->s_ScreenCoordY - 16 * radiusY)
            return true;*/
    if(testCollision(player->s_ScreenCoordX - 16 * radiusX, player->s_ScreenCoordY - 16 * radiusY, mnst->s_CoordX, mnst->s_CoordY, Square(0, 0, s_GameClass->s_DisplayStruct->s_GameResolutionX + 2 * 16 * radiusX, s_GameClass->s_DisplayStruct->s_GameResolutionY + 2 * 16 * radiusY), mnst->getCollision())) return true;
    return false;
}

void FactoryMonsters::live()
{
    bool testChangeLevel = false, testNoChangeLevel = false;
    map<int, CreatureMonster*>::iterator iter, iter2;
    for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
        /*if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_GameResolutionX + s_GameClass->s_GameInfo->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
            iter->second->s_CoordX >= s_GameClass->s_GameInfo->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
            iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_GameResolutionY + s_GameClass->s_GameInfo->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY &&
            iter->second->s_CoordY >= s_GameClass->s_GameInfo->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY)*/
        if(isMonsterInPlayerRadius(s_GameClass->s_GameInfo->s_MyPlayer, iter->second, MDRT_LIVE))
                if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                {
                    /*if(iter->second->s_Activated == false && iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_GameResolutionX + s_GameClass->s_GameInfo->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                        iter->second->s_CoordX >= s_GameClass->s_GameInfo->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                        iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_GameResolutionY + s_GameClass->s_GameInfo->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY &&
                        iter->second->s_CoordY >= s_GameClass->s_GameInfo->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY)*/
                    if(iter->second->s_Activated == false && isMonsterInPlayerRadius(s_GameClass->s_GameInfo->s_MyPlayer, iter->second, MDRT_ACTIVATE))
                        iter->second->s_Activated = true;
                    if(iter->second->s_Activated == true)
                    {
                        iter->second->live();
                        iter->second->s_IsAlwaysLiveInStep = true;
                    }
                }
    map<int, CreaturePlayer*>::iterator iter1;
    for(iter1 = s_GameClass->s_GameInfo->s_Players.begin(); iter1 != s_GameClass->s_GameInfo->s_Players.end(); iter1++)
    {
        for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
            /*if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_GameResolutionX + iter1->second->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
                iter->second->s_CoordX >= iter1->second->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
                iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_GameResolutionY + iter1->second->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY &&
                iter->second->s_CoordY >= iter1->second->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY)*/
            if(isMonsterInPlayerRadius(iter1->second, iter->second, MDRT_LIVE))
                    if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                    {
                        /*if(iter->second->s_Activated == false && iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_GameResolutionX + iter1->second->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                            iter->second->s_CoordX >= iter1->second->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                            iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_GameResolutionY + iter1->second->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY &&
                            iter->second->s_CoordY >= iter1->second->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY)*/
                        if(iter->second->s_Activated == false && isMonsterInPlayerRadius(iter1->second, iter->second, MDRT_ACTIVATE))
                            iter->second->s_Activated = true;
                        if(iter->second->s_Activated == true)
                        {
                            iter->second->live();
                            iter->second->s_IsAlwaysLiveInStep = true;
                        }
                    }
    }
    for(iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2;)
    {
       iter->second->s_IsAlwaysLiveInStep = false;
       if(iter->second->s_DeleteNow == true)
       {
           if(iter->second->s_AIMonsterValues["GS_type"] == "boss") testChangeLevel = true;
           else if(iter->second->s_AIMonsterValues["GS_type"] == "" && s_GameClass->s_Data->s_Monsters->s_MonstersInfo[iter->second->s_Number - 1]->getValue("options", "type") == "boss") testChangeLevel = true;
           delete iter->second;
           s_Monsters.erase(iter++);
       }
       else
       {
           if(iter->second->s_AIMonsterValues["GS_type"] == "boss") testNoChangeLevel = true;
           else if(iter->second->s_AIMonsterValues["GS_type"] == "" && s_GameClass->s_Data->s_Monsters->s_MonstersInfo[iter->second->s_Number - 1]->getValue("options", "type") == "boss") testNoChangeLevel = true;
           ++iter;
       }
    }
    addMonstersFromQueue();
    if(testNoChangeLevel == true) testChangeLevel = false;
    if(testChangeLevel == true) s_GameClass->changeNextLevel();
    if(s_GameClass->s_GameInfo->s_ForcedChangeLevel == true) s_GameClass->changeLevel(s_GameClass->s_GameInfo->s_ChangeLevelTo);
}

void FactoryMonsters::clear()
{
    map<int, CreatureMonster*>::iterator iter, iter2;
    for (iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2;)
    {
       delete iter->second;
       s_Monsters.erase(iter++);
    }
    s_AIMonstersValues->clear();
    s_QueueForAddingMonsters.clear();
    s_MaxIndex = 0;
}

void FactoryMonsters::reloadAIAll()
{
    map<int, CreatureMonster*>::iterator iter, iter2;
    for (iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2; iter++) s_GameClass->s_AI->loadAI(iter->second);
}

void FactoryMonsters::drawAll()
{
    map<int, CreatureMonster*>::iterator iter;
    for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++) if(iter->second->s_DeleteNow == false) iter->second->draw();
}
