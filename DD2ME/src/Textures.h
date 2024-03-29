#ifndef TEXTURES_H
#define TEXTURES_H
#include <vector>

#include "IniParser/PostParsingStruct.h"

#include <SFML/Graphics.hpp>

class Game;

struct TileAnimationStepInfo
{
    int s_TileID;
    unsigned int s_AnimationSleep;
};

struct TileAnimationInfo
{
    std::vector<TileAnimationStepInfo> s_TileFrames;
    int s_GeneralAnimationSteps = 0;
};

class Textures
{
    public:
        Textures(Game*);
        ~Textures();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_TilesInfo;
        std::map<int, std::map<int, TileAnimationInfo> > s_TilesAnimationInfo;
        std::map<int, sf::Texture*> s_Tiles;
        std::map<int, std::map<int,int> > s_MaskTiles;
        std::map<int, std::map<int, sf::Sprite*> > s_CacheTiles;
        std::map<int, IniParser::PostParsingStruct*> s_DeathTilesInfo;
        std::map<int, std::map<int, sf::Texture*> > s_DeathTiles;
        std::map<int, std::map<int, sf::Sprite*> > s_CacheDeathTiles;
        bool s_CacheCreated;
        bool load(std::string);
        bool createCache();
        void drawTile(int, int, int, int, int);
        int getFrame(int, int, int);
        int getTileIDByFrame(int, int);
        int getCurrentAnimationTileID(int, int, int);
        void deleteAllGDIObjects();
        void createMaskTransparent(int, int, int);
};
#endif
