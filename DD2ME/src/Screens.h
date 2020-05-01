#ifndef SCREENS_H
#define SCREENS_H
#include <map>
#include "canvas.hpp"

#include "IniParser/PostParsingStruct.h"

using namespace std;
using namespace cnv;

class Game;

class Screens
{
    public:
        Screens(Game*);
        ~Screens();
        Game* s_GameClass;
        PostParsingStruct* s_ScreensInfo;
        PostParsingStruct* s_ChangeLevelInfo;
        PostParsingStruct* s_MainScreenInfo;
        map<string, Bitmap*> s_StandardScreens;
        map<string, map<int, Bitmap*> > s_MCScreens;
        bool s_CacheCreated;
        map<string, Bitmap*> s_CacheStandardScreens;
        map<string, map<int, Bitmap*> > s_CacheMCScreens;
        bool load(string);
        bool createCache();
        void drawScreen(string name, int x, int y, int number = 0);
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
