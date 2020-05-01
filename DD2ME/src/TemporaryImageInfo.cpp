#include "TemporaryImageInfo.h"

#include "Game.h"

TemporaryImageInfo::TemporaryImageInfo():
    s_CoordX(0),
    s_CoordY(0),
    s_TimeLive(0),
    s_ChangeX(0),
    s_ChangeY(0),
    s_Bitmaps(0),
    s_CacheBitmaps(0),
    s_Bitmap(0),
    s_Cache(0),
    s_NumberOfFrames(0),
    s_Type(""),
    s_Animated(false),
    s_CacheCreated(false),
    s_ManyFrames(false),
    s_GameClass(0)
{
    //...
}

TemporaryImageInfo::~TemporaryImageInfo()
{
    //...
}

void TemporaryImageInfo::draw()
{
    int frame;
    if(s_Animated == true) frame = s_GameClass->s_AnimationStep%s_NumberOfFrames;
    else frame = 0;
    int x = s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX;
    int y = s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY;
    if(s_CacheCreated == false)
    {
        if(s_ManyFrames == true) s_GameClass->s_Window->draw(Image(Bitmap( (*(*s_Bitmaps)[frame]), 0, 0, (*s_Bitmaps)[frame]->width()/2, (*s_Bitmaps)[frame]->height()), x, y));
        else s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmap), 0, 0, s_Bitmap->width()/2, s_Bitmap->height()), x, y));
    }
    else
    {
        if(s_ManyFrames == true) s_GameClass->s_Window->draw( Image( (*(*s_CacheBitmaps)[frame]), x, y));
        else s_GameClass->s_Window->draw( Image( (*s_Cache), x, y));
    }
}

void TemporaryImageInfo::live()
{
    s_TimeLive--;
    s_CoordX += s_ChangeX;
    s_CoordY += s_ChangeY;
}
