#ifndef GUIDATA_H
#define GUIDATA_H
#include <map>
#include <SFML/Graphics.hpp>

#include "IniParser/PostParsingStruct.h"

class Game;

class GuiData
{
    public:
        GuiData();
        ~GuiData();
        IniParser::PostParsingStruct* s_FontsInfo;
        std::map<std::string, sf::Font> s_Fonts;
        bool load(std::string);
        sf::Font getFont(std::string);
};

#endif