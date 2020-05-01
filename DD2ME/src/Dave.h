#ifndef DAVE_H
#define DAVE_H
#include <map>
#include "canvas.hpp"
#include "Square.h"
#include "IniParser/PostParsingStruct.h"
using namespace cnv;
using namespace std;

class Game;

class Dave
{
    public:
        Dave(Game*);
        ~Dave();
        Game* s_GameClass;
        PostParsingStruct* s_DaveInfo;
        map<string, map<int, Bitmap*> > s_Bitmaps;
        map<string, map<int, Bitmap*> > s_CacheImages;
        map<string, map<int, Square> > s_Collisions;
        bool s_CacheCreated;
        bool load(string);
        bool createCache();
        void drawDave(string,int,int,int);
        void drawBandolier(int,int,int);
        void collisionAnalyze(string,int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
