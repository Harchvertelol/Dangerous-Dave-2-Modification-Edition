#include "Monsters.h"

#include "Game.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

Monsters::Monsters(Game* gameclass):
    s_GameClass(gameclass),
    s_GlobMonstersInfo(0),
    s_CacheCreated(false)
{
    //...
}

Monsters::~Monsters()
{
    if(s_GlobMonstersInfo != 0) delete s_GlobMonstersInfo;
    map<int, PostParsingStruct*>::iterator iter, iter2;
    for (iter = s_MonstersInfo.begin(), iter2 = s_MonstersInfo.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    deleteAllGDIObjects();
}

void Monsters::deleteAllGDIObjects()
{
    map<int, map<string, map<int, Bitmap*> > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<string, map<int, Bitmap*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            map<int, Bitmap*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) delete _iter__->second;
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<string, map<int, Bitmap*> > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<string, map<int, Bitmap*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            map<int, Bitmap*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) delete _iter__->second;
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter__;
    }
}

bool Monsters::load(string PathToMonsterPack)
{
    int numberofmonsters;
    ParserInfoFile prs;
    s_GlobMonstersInfo = prs.getParsedFromFile(PathToMonsterPack + "monsters.dat");
    if(!s_GlobMonstersInfo) return false;
    numberofmonsters = atoi( s_GlobMonstersInfo->getValue("info","numberofmonsters").c_str() );
    for(int i = 0; i < numberofmonsters; i++)
        if( !loadMonster(PathToMonsterPack + itos(i+1) + "/", i ) ) return false;
    return true;
}

bool Monsters::loadMonster(string PathToMonster, int number)
{
    int numberofframes;
    string state;
    ParserInfoFile prs;
    s_MonstersInfo[number] = prs.getParsedFromFile(PathToMonster + "monster.info");
    if(!s_MonstersInfo[number]) return false;
    map<string, string>::iterator iter;
    for ( iter = s_MonstersInfo[number]->getMapVariables()["info"].begin(); iter != s_MonstersInfo[number]->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            s_Bitmaps[number][state][i] = new Bitmap(PathToMonster + state + "_" + itos(i+1) + ".bmp");
            if(s_Bitmaps[number][state][i]->empty()) return false;
            collisionAnalyze(number,state,i);
        }
    }
    numberofframes = atoi( s_MonstersInfo[number]->getValue("other","numberofframesdeathtiles").c_str() );
    for(int i = 0; i < numberofframes; i++)
    {
        s_Bitmaps[number]["deathtiles"][i] = new Bitmap(PathToMonster + "DeathTiles/" + itos(i+1) + ".bmp");
        if(s_Bitmaps[number]["deathtiles"][i]->empty()) return false;
    }
    return true;
}

bool Monsters::createCache()
{
    cout<<"Creating monsters cache..."<<endl;
    int numberofframes, xSize, ySize;
    string state;
    int numberofmonsters = atoi( s_GlobMonstersInfo->getValue("info","numberofmonsters").c_str() );
    for(int number = 0; number < numberofmonsters; number++)
    {
        map<string, string>::iterator iter;
        for ( iter = s_MonstersInfo[number]->getMapVariables()["info"].begin(); iter != s_MonstersInfo[number]->getMapVariables()["info"].end(); iter++ )
        {
            state = iter->first.substr(14);
            numberofframes = atoi( iter->second.c_str() );
            for(int i = 0; i < numberofframes; i++)
            {
                xSize = s_Bitmaps[number][state][i]->width()/2;
                ySize = s_Bitmaps[number][state][i]->height();
                s_CacheImages[number][state][i] = new Bitmap( *s_Bitmaps[number][state][i], 0, 0, xSize, ySize);
            }
            numberofframes = atoi( s_MonstersInfo[number]->getValue("other","numberofframesdeathtiles").c_str() );
            for(int i = 0; i < numberofframes; i++)
            {
                xSize = s_Bitmaps[number]["deathtiles"][i]->width();
                ySize = s_Bitmaps[number]["deathtiles"][i]->height();
                s_CacheImages[number]["deathtiles"][i] = new Bitmap( *s_Bitmaps[number]["deathtiles"][i], 0, 0, xSize, ySize);
            }
        }
    }
    s_CacheCreated = true;
    cout<<"Monsters cache created."<<endl;
    return true;
}

void Monsters::collisionAnalyze(int number, string state, int frame)
{
    bool first = true;
    int xSize = s_Bitmaps[number][state][frame]->width()/2;
    int ySize = s_Bitmaps[number][state][frame]->height();
    Bitmap dopBitmap( (*s_Bitmaps[number][state][frame]), xSize, 0, xSize, ySize );
    for(int i = 0; i < ySize; i++)
        for(int j = 0; j < xSize; j++)
        {
            if(dopBitmap.get_pixel_r(j,i) == 252 && first == true)
            {
                s_Collisions[number][state][frame].s_XL = j;
                s_Collisions[number][state][frame].s_YL = i;
                first = false;
            }
            else if(dopBitmap.get_pixel_r(j,i) == 252 && first == false)
            {
                s_Collisions[number][state][frame].s_XR = j;
                s_Collisions[number][state][frame].s_YR = i;
            }
        }
}

void Monsters::drawMonster(int number, string anim, int frame, int x, int y)
{
    if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmaps[number - 1][anim][frame]), 0, 0, s_Bitmaps[number - 1][anim][frame]->width()/2, s_Bitmaps[number - 1][anim][frame]->height()), x, y));
    else s_GameClass->s_Window->draw( Image( (*s_CacheImages[number - 1][anim][frame]), x, y) );
}

void Monsters::createMaskTransparent(int r, int g, int b)
{
    map<int, map<string, map<int, Bitmap*> > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<string, map<int, Bitmap*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            map<int, Bitmap*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) _iter__->second->create_mask(r, g, b);
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<string, map<int, Bitmap*> > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<string, map<int, Bitmap*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            map<int, Bitmap*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) _iter__->second->create_mask(r, g, b);
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter__;
    }
}
