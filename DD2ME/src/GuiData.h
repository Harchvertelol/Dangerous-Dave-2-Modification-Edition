#ifndef GUIDATA_H
#define GUIDATA_H
#include <map>
#include <SFML/Graphics.hpp>
#include <TGUI/Font.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class GuiData
{
    public:
        GuiData(Game*);
        ~GuiData();
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_GuiInfo;
        std::map<std::string, sf::Font> s_Fonts;
        bool load(std::string);
        bool isExists(std::string);
        sf::Font getSFMLFont(std::string);
        tgui::Font getTGUIFont(std::string);
};

#endif
