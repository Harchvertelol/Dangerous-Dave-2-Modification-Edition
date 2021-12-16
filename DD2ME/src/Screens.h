#ifndef SCREENS_H
#define SCREENS_H
#include <map>
#include <SFML/Graphics.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class Screens
{
    public:
        Screens(Game*);
        ~Screens();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_ScreensInfo;
        IniParser::PostParsingStruct* s_ChangeLevelInfo;
        IniParser::PostParsingStruct* s_MainScreenInfo;
        std::map<std::string, sf::Texture*> s_StandardScreens;
        std::map<std::string, std::map<int, sf::Texture*> > s_MCScreens;
        bool s_CacheCreated;
        std::map<std::string, sf::Sprite*> s_CacheStandardScreens;
        std::map<std::string, std::map<int, sf::Sprite*> > s_CacheMCScreens;
        bool load(std::string);
        bool createCache();
        void drawScreen(std::string name, int x, int y, int number = 0);
        void deleteAllGDIObjects();
        void createMaskTransparent(int, int, int);
};
#endif
