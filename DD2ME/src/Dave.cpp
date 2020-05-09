#include "Dave.h"

#include "Game.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

using namespace WorkFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

Dave::Dave(Game* gameclass):
    s_GameClass(gameclass),
    s_DaveInfo(0),
    s_CacheCreated(false)
{
    //...
}

Dave::~Dave()
{
    if(s_DaveInfo != 0) delete s_DaveInfo;
    deleteAllGDIObjects();
}

void Dave::deleteAllGDIObjects()
{
    map<string, map<int, Bitmap* > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<string, map<int, Bitmap* > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter__;
    }
}

bool Dave::load(string PathToDavePack)
{
    string state = "";
    int numberofframes = 0;
    ParserInfoFile prs;
    s_DaveInfo = prs.getParsedFromFile(PathToDavePack + "dave.dat");
    if(!s_DaveInfo) return false;
    map<string, string>::iterator iter;
    for ( iter = s_DaveInfo->getMapVariables()["info"].begin(); iter != s_DaveInfo->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            s_Bitmaps[state][i] = new Bitmap(PathToDavePack + state + "_" + itos(i+1) + ".bmp");
            if(s_Bitmaps[state][i]->empty()) return false;
            collisionAnalyze(state,i);
        }
    }
    for(int i = 0; i < 9; i++) s_Bitmaps["bandolier"][i] = new Bitmap(PathToDavePack + "bandolier_" + itos(i) + ".bmp" );
    return true;
}

bool Dave::createCache()
{
    cout<<"Creating dave cache..."<<endl;

    int numberofframes, xSize, ySize;
    string state;
    map<string, string>::iterator iter;
    for ( iter = s_DaveInfo->getMapVariables()["info"].begin(); iter != s_DaveInfo->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            xSize = s_Bitmaps[state][i]->width()/2;
            ySize = s_Bitmaps[state][i]->height();
            s_CacheImages[state][i] = new Bitmap( *s_Bitmaps[state][i], 0, 0, xSize, ySize);
        }
    }
    for(int i = 0; i < 9; i++)
    {
        xSize = s_Bitmaps["bandolier"][i]->width()/2 - 3;
        ySize = s_Bitmaps["bandolier"][i]->height();
        s_CacheImages["bandolier"][i] = new Bitmap( *s_Bitmaps["bandolier"][i], 0, 0, xSize, ySize);
    }
    s_CacheCreated = true;
    cout<<"Dave cache created."<<endl;
    return true;
}

void Dave::drawDave(string anim, int frame, int x, int y)
{
    if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmaps[anim][frame]), 0, 0, s_Bitmaps[anim][frame]->width()/2, s_Bitmaps[anim][frame]->height()), x, y));
    else s_GameClass->s_Window->draw( Image( (*s_CacheImages[anim][frame]), x, y) );
}

void Dave::drawBandolier(int frame, int x, int y)
{
    string anim = "bandolier";
    if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmaps[anim][frame]), 0, 0, s_Bitmaps[anim][frame]->width()/2, s_Bitmaps[anim][frame]->height()), x, y));
    else s_GameClass->s_Window->draw( Image( (*s_CacheImages[anim][frame]), x, y) );
}

void Dave::collisionAnalyze(string anim, int frame)
{
    bool first = true;
    int xSize = s_Bitmaps[anim][frame]->width()/2;
    int ySize = s_Bitmaps[anim][frame]->height();
    Bitmap dopBitmap( (*s_Bitmaps[anim][frame]), xSize, 0, xSize, ySize );
    for(int i = 0; i < ySize; i++)
        for(int j = 0; j < xSize; j++)
        {
            if(dopBitmap.get_pixel_r(j,i) == 252 && first == true)
            {
                s_Collisions[anim][frame].s_XL = j;
                s_Collisions[anim][frame].s_YL = i;
                first = false;
            }
            else if(dopBitmap.get_pixel_r(j,i) == 252 && first == false)
            {
                s_Collisions[anim][frame].s_XR = j;
                s_Collisions[anim][frame].s_YR = i;
            }
        }
}

void Dave::createMaskTransparent(int r, int g, int b)
{
    map<string, map<int, Bitmap* > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter_;
    }
    map<string, map<int, Bitmap* > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter__;
    }
}
