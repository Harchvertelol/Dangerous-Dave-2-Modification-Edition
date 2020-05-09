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

int FactoryMonsters::addMonster(int number, int x, int y, bool getstate)
{
    s_Monsters[s_MaxIndex] = new CreatureMonster(s_GameClass, number, getstate);
    s_Monsters[s_MaxIndex]->s_CoordX = x;
    s_Monsters[s_MaxIndex]->s_CoordY = y;
    s_MaxIndex++;
    return s_MaxIndex - 1;
}

void FactoryMonsters::live()
{
    bool testChangeLevel = false, testNoChangeLevel = false;
    map<int, CreatureMonster*>::iterator iter, iter2;
    for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
        if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + s_GameClass->s_GameInfo->s_ScreenCoordX + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
            iter->second->s_CoordX >= s_GameClass->s_GameInfo->s_ScreenCoordX - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
            iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + s_GameClass->s_GameInfo->s_ScreenCoordY + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ) &&
            iter->second->s_CoordY >= s_GameClass->s_GameInfo->s_ScreenCoordY - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ))
                    if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                    {
                        iter->second->live();
                        iter->second->s_IsAlwaysLiveInStep = true;
                    }
    map<int, CreatureDave*>::iterator iter1;
    for(iter1 = s_GameClass->s_GameInfo->s_Daves.begin(); iter1 != s_GameClass->s_GameInfo->s_Daves.end(); iter1++)
    {
        for ( iter = s_Monsters.begin(); iter != s_Monsters.end(); iter++)
            if(iter->second->s_CoordX <= s_GameClass->s_DisplayStruct->s_ResolutionX + iter1->second->s_ScreenCoordX + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
                iter->second->s_CoordX >= iter1->second->s_ScreenCoordX - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersX").c_str() ) &&
                iter->second->s_CoordY <= s_GameClass->s_DisplayStruct->s_ResolutionY + iter1->second->s_ScreenCoordY + 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ) &&
                iter->second->s_CoordY >= iter1->second->s_ScreenCoordY - 16*atoi(s_GameClass->s_IniFile->getValue("settings","distancelivemonstersY").c_str() ))
                        if(iter->second->s_DeleteNow == false && iter->second->s_IsAlwaysLiveInStep == false)
                        {
                            iter->second->live();
                            iter->second->s_IsAlwaysLiveInStep = true;
                        }
    }
    for(iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2;)
    {
       iter->second->s_IsAlwaysLiveInStep = false;
       if(iter->second->s_DeleteNow == true)
       {
           if(s_GameClass->s_Data->s_Monsters->s_MonstersInfo[iter->second->s_Number - 1]->getValue("options", "type") == "boss") testChangeLevel = true;
           delete iter->second;
           s_Monsters.erase(iter++);
       }
       else
       {
           if(s_GameClass->s_Data->s_Monsters->s_MonstersInfo[iter->second->s_Number - 1]->getValue("options", "type") == "boss") testNoChangeLevel = true;
           ++iter;
       }
    }
    if(testNoChangeLevel == true) testChangeLevel = false;
    if(testChangeLevel == true) s_GameClass->changeNextLevel();
}

void FactoryMonsters::clear()
{
    map<int, CreatureMonster*>::iterator iter, iter2;
    for (iter = s_Monsters.begin(), iter2 = s_Monsters.end(); iter != iter2;)
    {
       delete iter->second;
       s_Monsters.erase(iter++);
    }
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
