#include "Bonuses.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunction.h"

#include "Game.h"

Bonuses::Bonuses(Game* gameclass):
    s_GameClass(gameclass),
    s_GlobBonusesInfo(0),
    s_CacheCreated(false)
{
    //...
}

Bonuses::~Bonuses()
{
    if(s_GlobBonusesInfo != 0) delete s_GlobBonusesInfo;
    map<int, PostParsingStruct*>::iterator iter, iter2;
    for (iter = s_BonusesInfo.begin(), iter2 = s_BonusesInfo.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    deleteAllGDIObjects();
}

void Bonuses::deleteAllGDIObjects()
{
    map<int, map<int, Bitmap* > >::iterator iter_, iter2_;
    for (iter_ = s_BonusesBitmaps.begin(), iter2_ = s_BonusesBitmaps.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<int, Bitmap* > >::iterator iter__, iter2__;
    for (iter__ = s_BonusesCache.begin(), iter2__ = s_BonusesCache.end(); iter__ != iter2__;)
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

bool Bonuses::load(string PathToBonusPack)
{
    int numberofframes;
    ParserInfoFile prs;
    s_GlobBonusesInfo = prs.getParsedFromFile(PathToBonusPack + "bonuses.dat");
    if(!s_GlobBonusesInfo) return false;
    int numberofbonuses = atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() );
    for(int i = 0; i < numberofbonuses; i++)
    {
        s_BonusesInfo[i] = prs.getParsedFromFile(PathToBonusPack + WorkFunction::ConvertFunction::itos(i+1) + ".info");
        if(!s_BonusesInfo[i]) return false;
        s_BonusesBitmaps[i][0] = new Bitmap(PathToBonusPack + WorkFunction::ConvertFunction::itos(i+1) + ".bmp");
        numberofframes = atoi ( s_BonusesInfo[i]->getValue("info", "numberofframes").c_str() );
        for(int j = 0; j < numberofframes; j++)
        {
            s_BonusesBitmaps[i][j+1] = new Bitmap(PathToBonusPack + WorkFunction::ConvertFunction::itos(i+1) + "_" + WorkFunction::ConvertFunction::itos(j+1) + ".bmp");
            collisionAnalyze(i, j+1);
        }
        s_BonusesBitmaps[i][numberofframes + 1] = new Bitmap(PathToBonusPack + WorkFunction::ConvertFunction::itos(i+1) + "_p.bmp");
        collisionAnalyze(i, numberofframes + 1);
    }
    return true;
}

void Bonuses::collisionAnalyze(int anim, int frame)
{
    bool first = true;
    int xSize = s_BonusesBitmaps[anim][frame]->width()/2;
    int ySize = s_BonusesBitmaps[anim][frame]->height();
    Bitmap dopBitmap( (*s_BonusesBitmaps[anim][frame]), xSize, 0, xSize, ySize );
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

void Bonuses::drawBonus(int bonus, int x, int y, bool anim)
{
    if(bonus == 0 || bonus > atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() ) ) return;
    int frame;
    int numberofframes = atoi ( s_BonusesInfo[bonus-1]->getValue("info", "numberofframes").c_str() );
    if(anim == true && s_GameClass->s_IniFile->getValue("video", "bonusanimation") == "true") frame = s_GameClass->s_AnimationStep%numberofframes + 1;
    else frame = 1;
    if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_BonusesBitmaps[bonus-1][frame]), 0, 0, s_BonusesBitmaps[bonus-1][frame]->width()/2, s_BonusesBitmaps[bonus-1][frame]->height()), x, y));
    else s_GameClass->s_Window->draw( Image( (*s_BonusesCache[bonus-1][frame]), x, y));
}

bool Bonuses::createCache()
{
    cout<<"Creating bonuses cache..."<<endl;
    int numberofframes, xSize, ySize;
    int numberofbonuses = atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() );
    for(int i = 0; i < numberofbonuses; i++)
    {
        numberofframes = atoi ( s_BonusesInfo[i]->getValue("info", "numberofframes").c_str() );
        for(int j = 0; j < numberofframes; j++)
        {
            xSize = s_BonusesBitmaps[i][j+1]->width()/2;
            ySize = s_BonusesBitmaps[i][j+1]->height();
            s_BonusesCache[i][j+1] = new Bitmap( (*s_BonusesBitmaps[i][j+1]), 0, 0, xSize, ySize);
        }
        xSize = s_BonusesBitmaps[i][numberofframes + 1]->width()/2;
        ySize = s_BonusesBitmaps[i][numberofframes + 1]->height();
        s_BonusesCache[i][numberofframes + 1] = new Bitmap( (*s_BonusesBitmaps[i][numberofframes + 1]), 0, 0, xSize, ySize);
    }
    s_CacheCreated = true;
    cout<<"Bonuses cache created."<<endl;
    return true;
}

void Bonuses::createMaskTransparent(int r, int g, int b)
{
    map<int, map<int, Bitmap* > >::iterator iter_, iter2_;
    for (iter_ = s_BonusesBitmaps.begin(), iter2_ = s_BonusesBitmaps.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<int, Bitmap* > >::iterator iter__, iter2__;
    for (iter__ = s_BonusesCache.begin(), iter2__ = s_BonusesCache.end(); iter__ != iter2__;)
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
