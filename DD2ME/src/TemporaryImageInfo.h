#ifndef TEMPORARYIMAGEINFO_H
#define TEMPORARYIMAGEINFO_H
#include <iostream>
#include "canvas.hpp"
using namespace std;
using namespace cnv;

class Game;

class TemporaryImageInfo
{
    public:
        TemporaryImageInfo();
        ~TemporaryImageInfo();
        int s_CoordX;
        int s_CoordY;
        int s_TimeLive;
        int s_ChangeX;
        int s_ChangeY;
        map<int, Bitmap*>* s_Bitmaps;
        map<int, Bitmap*>* s_CacheBitmaps;
        Bitmap* s_Bitmap;
        Bitmap* s_Cache;
        int s_NumberOfFrames;
        string s_Type;
        bool s_Animated;
        bool s_CacheCreated;
        bool s_ManyFrames;
        Game* s_GameClass;
        void draw();
        void live();
};
#endif
