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
        IniParser::PostParsingStruct* s_TilesInfo;
        std::map<int, Bitmap*> s_Tiles;
        std::map<int, std::map<int,int> > s_MaskTiles;
        std::map<int, std::map<int, Bitmap*> > s_CacheTiles;
        std::map<int, IniParser::PostParsingStruct*> s_DeathTilesInfo;
        std::map<int, std::map<int, Bitmap*> > s_DeathTiles;
        std::map<int, std::map<int, Bitmap*> > s_CacheDeathTiles;
        bool s_CacheCreated;
        bool load(std::string);
        bool createCache();
        void drawTile(int,int,int);
        int getFrame();
        void deleteAllGDIObjects();
        void createMaskTransparent(int,int,int);
};
#endif
