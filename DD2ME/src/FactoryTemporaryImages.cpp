#include "FactoryTemporaryImages.h"

#include "Game.h"

FactoryTemporaryImages::FactoryTemporaryImages(Game* gameclass):
    s_GameClass(gameclass),
    s_MaxIndex(0)
{
    //...
}

FactoryTemporaryImages::~FactoryTemporaryImages()
{
    //...
}

void FactoryTemporaryImages::addImage(map<int, Bitmap*>* bmp, map<int, Bitmap*>* img, int coordX, int coordY, int timeLive, int changeX, int changeY, int numberofframes, string type, bool animated)
{
    s_Info[s_MaxIndex].s_GameClass = s_GameClass;
    s_Info[s_MaxIndex].s_ManyFrames = true;
    if(img != 0) s_Info[s_MaxIndex].s_CacheCreated = true;
    s_Info[s_MaxIndex].s_NumberOfFrames = numberofframes;
    s_Info[s_MaxIndex].s_Bitmaps = bmp;
    s_Info[s_MaxIndex].s_CacheBitmaps = img;
    s_Info[s_MaxIndex].s_CoordX = coordX;
    s_Info[s_MaxIndex].s_CoordY = coordY;
    s_Info[s_MaxIndex].s_TimeLive = timeLive;
    s_Info[s_MaxIndex].s_ChangeX = changeX;
    s_Info[s_MaxIndex].s_ChangeY = changeY;
    s_Info[s_MaxIndex].s_Type = type;
    s_Info[s_MaxIndex].s_Animated = animated;
    s_MaxIndex++;
}

void FactoryTemporaryImages::addImage(Bitmap* bmp, Bitmap* img, int coordX, int coordY, int timeLive, int changeX, int changeY, string type)
{
    s_Info[s_MaxIndex].s_GameClass = s_GameClass;
    s_Info[s_MaxIndex].s_ManyFrames = false;
    if(img != 0) s_Info[s_MaxIndex].s_CacheCreated = true;
    s_Info[s_MaxIndex].s_Bitmap = bmp;
    s_Info[s_MaxIndex].s_Cache = img;
    s_Info[s_MaxIndex].s_CoordX = coordX;
    s_Info[s_MaxIndex].s_CoordY = coordY;
    s_Info[s_MaxIndex].s_TimeLive = timeLive;
    s_Info[s_MaxIndex].s_ChangeX = changeX;
    s_Info[s_MaxIndex].s_ChangeY = changeY;
    s_Info[s_MaxIndex].s_Type = type;
    s_MaxIndex++;
}

void FactoryTemporaryImages::removeImage(int key)
{
    s_Info.erase(key);
}

void FactoryTemporaryImages::live()
{
    map<int, TemporaryImageInfo>::iterator iter;
    for ( iter = s_Info.begin(); iter != s_Info.end();)
    {
        iter->second.live();
        if(iter->second.s_TimeLive == 0)
        {
            map<int, TemporaryImageInfo>::iterator next(iter);
            ++next;
            s_Info.erase(iter);
            iter = next;
        }
        else ++iter;
    }
}

void FactoryTemporaryImages::drawAll()
{
    map<int, TemporaryImageInfo>::iterator iter;
    for ( iter = s_Info.begin(); iter != s_Info.end(); iter++ ) iter->second.draw();
}

void FactoryTemporaryImages::clear()
{
    for(int i = 0; i < s_MaxIndex; i++) removeImage(i);
}
