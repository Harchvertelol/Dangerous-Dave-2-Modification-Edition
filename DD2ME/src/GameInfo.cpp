#include "GameInfo.h"

#include "Defines.h"

#include "Game.h"

using namespace std;

using namespace sf;

GameInfo::GameInfo(Game* gameclass):
    s_GameClass(gameclass),
    s_CurrentLevel(1),
    s_ChangeLevelTo(0),
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
    s_KeyInfo(0),
    s_OldAnSt(0),
    s_CurrentDistanceLiveMonstersX(0),
    s_CurrentDistanceLiveMonstersY(0),
    s_CurrentDistanceActivateMonstersX(0),
    s_CurrentDistanceActivateMonstersY(0),
    s_ForcedChangeLevel(false),
    s_CheatGod(false),
    s_IsAIOn(true),
    s_IsGhostOn(false),
    s_IsInfoShow(false)
{
    s_MyPlayer = new CreaturePlayer(gameclass);
    s_FactoryMonsters = new FactoryMonsters(gameclass);
}

GameInfo::~GameInfo()
{
    if(s_MyPlayer != 0) delete s_MyPlayer;
    if(s_FactoryMonsters != 0) delete s_FactoryMonsters;
    map<int, CreaturePlayer*>::iterator iter_, iter2_;
    for (iter_ = s_Players.begin(), iter2_ = s_Players.end(); iter_ != iter2_;)
    {
        if(iter_->second != 0) delete iter_->second;
        ++iter_;
    }
}

void GameInfo::readKeys(IniParser::PostParsingStruct* pps)
{
    s_KeyDown = atoi( pps->getValue("keys","down").c_str() );
    s_KeyUp = atoi( pps->getValue("keys","up").c_str() );
    s_KeyRight = atoi( pps->getValue("keys","right").c_str() );
    s_KeyLeft = atoi( pps->getValue("keys","left").c_str() );
    s_KeyLeftDown = atoi( pps->getValue("keys","leftdown").c_str() );
    s_KeyLeftUp = atoi( pps->getValue("keys","leftup").c_str() );
    s_KeyRightDown = atoi( pps->getValue("keys","rightdown").c_str() );
    s_KeyRightUp = atoi( pps->getValue("keys","rightup").c_str() );
    s_KeyShoot = atoi( pps->getValue("keys","shoot").c_str() );
    s_KeyJump = atoi( pps->getValue("keys","jump").c_str() );
    s_KeyConsole = atoi( pps->getValue("keys","console").c_str() );
    s_KeyInfo = atoi( pps->getValue("keys","info").c_str() );
    s_KeySkip = atoi( pps->getValue("keys","skip").c_str() );
}

void GameInfo::live()
{
    s_GameClass->s_FactoryTmpImgs->live();
    s_FactoryMonsters->live();
    map<int, CreaturePlayer*>::iterator iter;
    for ( iter = s_Players.begin(); iter != s_Players.end(); iter++)
    {
        iter->second->live(false);
    }
    //...
    if(!s_Stop) s_MyPlayer->live();
}

bool GameInfo::deathPlayer(int type)
{
    if(type == 0) return false;
    if(s_CheatGod == true) return false;
    if(s_MyPlayer->s_State == "doorexit") return false;
    if(s_Stop) return false;
    s_Stop = true;
    if(type < 0)
    {
        int type_tmp = (-1)*type - 1;
        int SizeXLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() );
        int SizeYLev = atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeY") ).c_str() );
        int tile_x = type_tmp % SizeXLev;
        int tile_y = (type_tmp - tile_x) / SizeYLev;
        s_DeathType = (-1)*(s_GameClass->s_Data->s_Textures->getCurrentAnimationTileID(s_GameClass->s_Data->s_Level->s_Fields[s_GameClass->s_Data->s_Level->getNamePhysicTilesField()][type_tmp], tile_x, tile_y) + 1);
    }
    else s_DeathType = type;
    s_OldAnSt = s_GameClass->s_AnimationStep;
    s_DopFrame = -1;
    s_CurrentLives--;
    s_GameClass->s_Data->s_Sounds->stop("ammo");
    s_GameClass->s_Data->s_Sounds->play("death");
    return true;
}

int GameInfo::getChangeLevelTo(string change_level_to)
{
    int tmp_cur_lev = s_CurrentLevel;
    if(change_level_to == "") change_level_to = "0";
    bool forced = false;
    if(change_level_to.substr(change_level_to.size() - 1) == "f")
    {
        change_level_to = change_level_to.substr(0, change_level_to.size() - 1);
        forced = true;
    }
    int lev_tmp = atoi(change_level_to.c_str());
    if(change_level_to.find("+") != string::npos || change_level_to.find("-") != string::npos) tmp_cur_lev += lev_tmp;
    else tmp_cur_lev = lev_tmp;
    if(!forced)
    {
        if(tmp_cur_lev <= 0) tmp_cur_lev = 1;
        else if(tmp_cur_lev > atoi(s_GameClass->s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str())) tmp_cur_lev = atoi(s_GameClass->s_Data->s_LevelsInfo->getValue("info", "numberoflevels").c_str());
    }
    return tmp_cur_lev;
}

void GameInfo::doChangeLevelOnGameOver()
{
    s_CurrentLives = GC_START_LIVES_NUMBER;
    if(s_GameClass->s_Data->s_Level->s_Params->isExists("options", "changelevelongameover"))
    {
        string change_level_to = s_GameClass->s_Data->s_Level->s_Params->getValue("options", "changelevelongameover");
        s_CurrentLevel = getChangeLevelTo(change_level_to);
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
        type = type*(-1) - 1;
        numberofframes = atoi( s_GameClass->s_Data->s_Textures->s_DeathTilesInfo[type]->getValue("info", "numberofframes").c_str() );
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
        drawDeathFrame(&s_GameClass->s_Data->s_Textures->s_DeathTiles[type], &s_GameClass->s_Data->s_Textures->s_CacheDeathTiles[type], frame);
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
    int x = s_MyPlayer->s_CoordX + 8 - (*bt)[frame]->getSize().x / 2 - s_ScreenCoordX;
    int y = s_MyPlayer->s_CoordY + 16 - (*bt)[frame]->getSize().y / 2 - s_ScreenCoordY;
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

void GameInfo::correctionScreen(CreaturePlayer* player)
{
    //player->s_ScreenCoordX = player->s_CoordX - s_GameClass->s_DisplayStruct->s_GameResolutionX/2;
    //player->s_ScreenCoordY = player->s_CoordY - s_GameClass->s_DisplayStruct->s_GameResolutionY/2;

    if(player->s_CoordX - player->s_ScreenCoordX < atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXleft").c_str()) * 16) player->s_ScreenCoordX = player->s_CoordX - atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXleft").c_str()) * 16;
    else if( (player->s_ScreenCoordX + s_GameClass->s_DisplayStruct->s_GameResolutionX) - player->s_CoordX < (atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXright").c_str()) - 1) * 16) player->s_ScreenCoordX = player->s_CoordX + (atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenXright").c_str()) - 1) * 16 - s_GameClass->s_DisplayStruct->s_GameResolutionX;

    if(player->s_CoordY - player->s_ScreenCoordY < atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYup").c_str()) * 16) player->s_ScreenCoordY = player->s_CoordY - atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYup").c_str()) * 16;
    else if( (player->s_ScreenCoordY + s_GameClass->s_DisplayStruct->s_GameResolutionY) - player->s_CoordY < (2 + atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYdown").c_str())) * 16) player->s_ScreenCoordY = player->s_CoordY + (2 + atoi(s_GameClass->s_IniFile->getValue("settings", "limitshiftscreenYdown").c_str())) * 16 - s_GameClass->s_DisplayStruct->s_GameResolutionY;

    if(player->s_ScreenCoordX + s_GameClass->s_DisplayStruct->s_GameResolutionX > 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) - 32) player->s_ScreenCoordX = 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeX") ).c_str() ) - s_GameClass->s_DisplayStruct->s_GameResolutionX - 32;
    if(player->s_ScreenCoordX < 32) player->s_ScreenCoordX = 32;
    if(player->s_ScreenCoordY + s_GameClass->s_DisplayStruct->s_GameResolutionY > 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeY") ).c_str() ) - 32) player->s_ScreenCoordY = 16*atoi( ( s_GameClass->s_Data->s_Level->s_Params->getValue("info", "sizeY") ).c_str() ) - s_GameClass->s_DisplayStruct->s_GameResolutionY - 32;
    if(player->s_ScreenCoordY < 32) player->s_ScreenCoordY = 32;
}
