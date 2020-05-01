#ifndef TEXTURES_H
#define TEXTURES_H
#include "IniParser/PostParsingStruct.h"
#include "canvas.hpp"
using namespace cnv;

class Game;

class Textures
{
    public:
        Textures(Game*);
        ~Textures();
        Game* s_GameClass;
        PostParsingStruct* s_TilesInfo;
        map<int, Bitmap*> s_Tiles;
        map<int, map<int,int> > s_MaskTiles;
        map<int, map<int, Bitmap*> > s_CacheTiles;
        map<int, PostParsingStruct*> s_DeathTilesInfo;
        map<int, map<int, Bitmap*> > s_DeathTiles;
        map<int, map<int, Bitmap*> > s_CacheDeathTiles;
        bool s_CacheCreated;
        bool load(string);
        bool createCache();
        void drawTile(int,int,int);
        int getFrame();
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
