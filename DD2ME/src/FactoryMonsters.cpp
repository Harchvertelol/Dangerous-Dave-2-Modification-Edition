#include "FactoryMonsters.h"

#include "Game.h"

#include "WorkFunctions.h"
#include "Defines.h"

#include "Thread/Thread.h"

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

FactoryMonsters::FactoryMonsters(Game* gameclass):
    s_GameClass(gameclass),
    s_MaxIndex(0)
{
    //...
}

FactoryMonsters::~FactoryMonsters()
{
    clear();
}

void FactoryMonsters::removeMonster(int key)
{
    delete s_Monsters[key];
    s_Monsters.erase(key);
}

int FactoryMonsters::addMonsterImmediately(int number, int x, int y, bool getstate)
{
    s_Monsters[s_MaxIndex] = new CreatureMonster(s_GameClass, number, s_MaxIndex, getstate);
    s_Monsters[s_MaxIndex]->s_CoordX = x;
    s_Monsters[s_MaxIndex]->s_CoordY = y;
    s_MaxIndex++;
    return s_MaxIndex - 1;
}

CreatureMonster* FactoryMonsters::addMonster(int number, int x, int y, bool getstate)
{
    CreatureMonster* newMonster = new CreatureMonster(s_GameClass, number, s_MaxIndex + s_QueueForAddingMonsters.size(), getstate);
    newMonster->s_CoordX = x;
    newMonster->s_CoordY = y;
    s_QueueForAddingMonsters.push_back(newMonster);
    return newMonster;
}

void FactoryMonsters::addMonstersFromQueue()
{
    if(s_QueueForAddingMonsters.empty()) return;
    for(int i = 0; i < s_QueueForAddingMonsters.size(); i++)
    {
        s_Monsters[s_MaxIndex] = s_QueueForAddingMonsters[i];
        //s_Monsters[s_MaxIndex]->s_ID = s_MaxIndex;
        if(s_MaxIndex != s_QueueForAddingMonsters[i]->s_ID) cout << "Error with monster ID!" << endl;
        s_MaxIndex++;
    }
    s_QueueForAddingMonsters.clear();
}

bool FactoryMonsters::isMonsterInDaveRadius(CreatureDave* dave, CreatureMonster* mnst, MONSTER_DAVE_RADIUS_TYPE rad_type)
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
    /*if(mnst->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + dave->s_ScreenCoordX + 16 * radiusX &&
        mnst->s_CoordX >= dave->s_ScreenCoordX - 16 * radiusX &&
        mnst->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + dave->s_ScreenCoordY + 16 * radiusY &&
        mnst->s_CoordY >= dave->s_ScreenCoordY - 16 * radiusY)
            return true;*/
    if(testCollision(dave->s_ScreenCoordX - 16 * radiusX, dave->s_ScreenCoordY - 16 * radiusY, mnst->s_CoordX, mnst->s_CoordY, Square(0, 0, s_GameClass->s_DisplayStruct->s_ResolutionX + 2 * 16 * radiusX, s_GameClass->s_DisplayStruct->s_ResolutionY + 2 * 16 * radiusY), mnst->getCollision())) return true;
    return false;
}

void FactoryMonsters::live()
{
    bool testChangeLevel = false, testNoChangeLevel = false;
    map<int, CreatureMonster*>::iterator iter, iter2;
    for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
        /*if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + s_GameClass->s_GameInfo->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
            iter->second->s_CoordX >= s_GameClass->s_GameInfo->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
            iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + s_GameClass->s_GameInfo->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY &&
            iter->second->s_CoordY >= s_GameClass->s_GameInfo->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY)*/
        if(isMonsterInDaveRadius(s_GameClass->s_GameInfo->s_MyDave, iter->second, MDRT_LIVE))
                if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                {
                    /*if(iter->second->s_Activated == false && iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + s_GameClass->s_GameInfo->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                        iter->second->s_CoordX >= s_GameClass->s_GameInfo->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                        iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + s_GameClass->s_GameInfo->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY &&
                        iter->second->s_CoordY >= s_GameClass->s_GameInfo->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY)*/
                    if(iter->second->s_Activated == false && isMonsterInDaveRadius(s_GameClass->s_GameInfo->s_MyDave, iter->second, MDRT_ACTIVATE))
                        iter->second->s_Activated = true;
                    if(iter->second->s_Activated == true)
                    {
                        iter->second->live();
                        iter->second->s_IsAlwaysLiveInStep = true;
                    }
                }
    map<int, CreatureDave*>::iterator iter1;
    for(iter1 = s_GameClass->s_GameInfo->s_Daves.begin(); iter1 != s_GameClass->s_GameInfo->s_Daves.end(); iter1++)
    {
        for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
            /*if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + iter1->second->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
                iter->second->s_CoordX >= iter1->second->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersX &&
                iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + iter1->second->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY &&
                iter->second->s_CoordY >= iter1->second->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceLiveMonstersY)*/
            if(isMonsterInDaveRadius(iter1->second, iter->second, MDRT_LIVE))
                    if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                    {
                        /*if(iter->second->s_Activated == false && iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + iter1->second->s_ScreenCoordX + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                            iter->second->s_CoordX >= iter1->second->s_ScreenCoordX - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersX &&
                            iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + iter1->second->s_ScreenCoordY + 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY &&
                            iter->second->s_CoordY >= iter1->second->s_ScreenCoordY - 16 * s_GameClass->s_GameInfo->s_CurrentDistanceActivateMonstersY)*/
                        if(iter->second->s_Activated == false && isMonsterInDaveRadius(iter1->second, iter->second, MDRT_ACTIVATE))
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
    if(testChangeLevel == true || s_GameClass->s_GameInfo->s_ForcedChangeLevel == true) s_GameClass->changeNextLevel();
}

void FactoryMonsters::clear()
{
    map<int, CreatureMonster*>::iterator iter, iter2;
    for (iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2;)
    {
       delete iter->second;
       s_Monsters.erase(iter++);
    }
    s_AIMonstersValues.clear();
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
