#include "GameInfo.h"

#include "Defines.h"

#include "Game.h"

using namespace std;

using namespace sf;

GameInfo::GameInfo(Game* gameclass):
    s_GameClass(gameclass),
    s_CurrentLevel(1),
    s_GameState(0),
    s_CurrentLives(GC_START_LIVES_NUMBER),
    s_ScreenCoordX(0),
    s_ScreenCoordY(0),
    s_DopScreenCoordX(0),
    s_DopScreenCoordY(0),
    s_DopFrame(-1),
    s_Stop(false),
    s_DeathType(0),
    s_KeyLeft(0),
    s_KeyRight(0),
    s_KeyUp(0),
    s_KeyDown(0),
    s_KeyLeftUp(0),
    s_KeyRightUp(0),
    s_KeyLeftDown(0),
    s_KeyRightDown(0),
    s_KeyShoot(0),
    s_KeyJump(0),
    s_KeySkip(0),
    s_KeyConsole(0),
    s_OldAnSt(0),
    s_CurrentDistanceLiveMonstersX(0),
    s_CurrentDistanceLiveMonstersY(0),
    s_CurrentDistanceActivateMonstersX(0),
    s_CurrentDistanceActivateMonstersY(0),
    s_ForcedChangeLevel(false),
    s_CheatGod(false),
    s_IsAIOn(true),
    s_IsGhostOn(false)
{
    s_MyDave = new CreatureDave(gameclass);
    s_FactoryMonsters = new FactoryMonsters(gameclass);
}

GameInfo::~GameInfo()
{
    if(s_MyDave != 0) delete s_MyDave;
    if(s_FactoryMonsters != 0) delete s_FactoryMonsters;
    map<int, CreatureDave*>::iterator iter_, iter2_;
    for (iter_ = s_Daves.begin(), iter2_ = s_Daves.end(); iter_ != iter2_;)
    {
        if(iter_->second != 0) delete iter_->second;
        ++iter_;
    }
}

void GameInfo::live()
{
    s_GameClass->s_FactoryTmpImgs->live();
    s_FactoryMonsters->live();
    map<int, CreatureDave*>::iterator iter;
    for ( iter = s_Daves.begin(); iter != s_Daves.end(); iter++)
    {
        iter->second->live(false);
    }
    //...
    if(!s_Stop) s_MyDave->live();
}

bool GameInfo::deathDave(int type)
{
    if(type == 0) return false;
    if(s_CheatGod == true) return false;
    if(s_MyDave->s_State == "doorexit") return false;
    if(s_Stop) return false;
    s_Stop = true;
    s_DeathType = type;
    s_OldAnSt = s_GameClass->s_AnimationStep;
    s_DopFrame = -1;
    s_CurrentLives--;
    s_GameClass->s_Data->s_Sounds->stop("ammo");
    s_GameClass->s_Data->s_Sounds->play("death");
    return true;
}

void GameInfo::doChangeLevelOnGameOver()
{
    s_CurrentLives = GC_START_LIVES_NUMBER;
    if(s_GameClass->s_Data->s_Level->s_Params->isExists("options", "changelevelongameover"))
    {
        string change_level_to = s_GameClass->s_Data->s_Level->s_Params->getValue("options", "changelevelongameover");
        int lev_tmp = atoi(change_level_to.c_str());
        if(change_level_to.find("+") != string::npos || change_level_to.find("-") != string::npos) s_CurrentLevel += lev_tmp;
        else s_CurrentLevel = lev_tmp;
        if(s_CurrentLevel <= 0) s_CurrentLevel = 1;
        else if(s_CurrentLevel > atoi(s_GameClass->s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str())) s_CurrentLevel = atoi(s_GameClass->s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str());
    }
    else if(s_CurrentLevel > 1) s_CurrentLevel--;
    s_GameClass->s_Data->s_Sounds->play("gameover");
}

void GameInfo::playDeath()
{
    int type = s_DeathType;
    int frame = s_DopFrame;
    if(frame%2 != 0) frame--;
    frame = frame/2;
    if(frame < 0) frame = 0;
    int numberofframes;
    if(type < 0)
    {
        type = type*(-1);
        numberofframes = atoi( s_GameClass->s_Data->s_Textures->s_DeathTilesInfo[s_GameClass->s_Data->s_Level->s_Fields[s_GameClass->s_Data->s_Level->getNamePhysicTilesField()][type]]->getValue("info", "numberofframes").c_str() );
        if(frame == numberofframes + 1)
        {
            s_Stop = false;
            s_DopFrame = -1;
            s_DeathType = 0;
            s_OldAnSt = s_GameClass->s_AnimationStep;
            if(s_CurrentLives <= 0) doChangeLevelOnGameOver();
            s_GameClass->changeLevel(s_CurrentLevel, true, false);
            return;
        }
        if(frame >= numberofframes) frame = numberofframes - 1;
        drawDeathFrame(&s_GameClass->s_Data->s_Textures->s_DeathTiles[s_GameClass->s_Data->s_Level->s_Fields[s_GameClass->s_Data->s_Level->getNamePhysicTilesField()][type]], &s_GameClass->s_Data->s_Textures->s_CacheDeathTiles[s_GameClass->s_Data->s_Level->s_Fields[s_GameClass->s_Data->s_Level->getNamePhysicTilesField()][type]], frame);
    }
    else
    {
        type--;
        numberofframes = atoi( s_GameClass->s_Data->s_Monsters->s_MonstersInfo[type]->getValue("other", "numberofframesdeathtiles").c_str() );
        if(frame == numberofframes + 1)
        {
            s_Stop = false;
            s_DopFrame = -1;
            s_DeathType = 0;
            s_OldAnSt = s_GameClass->s_AnimationStep;
            if(s_CurrentLives <= 0) doChangeLevelOnGameOver();
            s_GameClass->changeLevel(s_CurrentLevel, true, false);
            return;
        }
        if(frame >= numberofframes) frame = numberofframes - 1;
        drawDeathFrame(&s_GameClass->s_Data->s_Monsters->s_Bitmaps[type]["deathtiles"], &s_GameClass->s_Data->s_Monsters->s_CacheImages[type]["deathtiles"], frame);
    }
    if(s_GameClass->s_AnimationStep != s_OldAnSt)
    {
        s_DopFrame++;
        s_OldAnSt = s_GameClass->s_AnimationStep;
    }
}

void GameInfo::drawDeathFrame(map<int, Texture*>* bt, map<int, Sprite*>* img, int frame)
{
    bool CacheCreated = false;
    if((*img)[0] != 0) CacheCreated = true;
    int x = s_MyDave->s_CoordX + 8 - (*bt)[frame]->getSize().x / 2 - s_ScreenCoordX;
    int y = s_MyDave->s_CoordY + 16 - (*bt)[frame]->getSize().y / 2 - s_ScreenCoordY;
    if(CacheCreated == false)
    {
        //s_GameClass->s_Window->draw(Image(Bitmap( (*(*bt)[frame]), 0, 0, (*bt)[frame]->width(), (*bt)[frame]->height()), x, y));
        Sprite spr((*(*bt)[frame]));
        spr.setTextureRect(IntRect(0, 0, (*bt)[frame]->getSize().x, (*bt)[frame]->getSize().y));
        spr.setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        //s_GameClass->s_Window->draw(Image((*(*img)[frame]), x, y));
        (*img)[frame]->setPosition(x, y);
        s_GameClass->s_RenderTexture->draw((*(*img)[frame]));
    }
}

void GameInfo::correctionScreen(CreatureDave* dave)
{
    //dave->s_ScreenCoordX = dave->s_CoordX - s_GameClass->s_DisplayStruct->s_GameResolutionX/2;
    //dave->s_ScreenCoordY = dave->s_CoordY - s_GameClass->s_DisplayStruct->s_GameResolutionY/2;

    if(dave->s_CoordX - dave->s_ScreenCoordX < atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXleft").c_str()) * 16) dave->s_ScreenCoordX = dave->s_CoordX - atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXleft").c_str()) * 16;
    else if( (dave->s_ScreenCoordX + s_GameClass->s_DisplayStruct->s_GameResolutionX) - dave->s_CoordX < (atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXright").c_str()) - 1) * 16) dave->s_ScreenCoordX = dave->s_CoordX + (atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXright").c_str()) - 1) * 16 - s_GameClass->s_DisplayStruct->s_GameResolutionX;

    if(dave->s_CoordY - dave->s_ScreenCoordY < atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYup").c_str()) * 16) dave->s_ScreenCoordY = dave->s_CoordY - atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYup").c_str()) * 16;
    else if( (dave->s_ScreenCoordY + s_GameClass->s_DisplayStruct->s_GameResolutionY) - dave->s_CoordY < (2 + atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYdown").c_str())) * 16) dave->s_ScreenCoordY = dave->s_CoordY + (2 + atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYdown").c_str())) * 16 - s_GameClass->s_DisplayStruct->s_GameResolutionY;

    if(dave->s_ScreenCoordX + s_GameClass->s_DisplayStruct->s_GameResolutionX > 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) - 32) dave->s_ScreenCoordX = 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) - s_GameClass->s_DisplayStruct->s_GameResolutionX - 32;
    if(dave->s_ScreenCoordX < 32) dave->s_ScreenCoordX = 32;
    if(dave->s_ScreenCoordY + s_GameClass->s_DisplayStruct->s_GameResolutionY > 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeY") ).c_str() ) - 32) dave->s_ScreenCoordY = 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeY") ).c_str() ) - s_GameClass->s_DisplayStruct->s_GameResolutionY - 32;
    if(dave->s_ScreenCoordY < 32) dave->s_ScreenCoordY = 32;
}
