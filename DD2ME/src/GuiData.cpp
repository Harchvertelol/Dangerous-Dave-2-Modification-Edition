#include "GuiData.h"

#include <iostream>

#include <TGUI/Backends/SFML/BackendFontSFML.hpp>

#include "IniParser/ParserInfoFile.h"

using namespace std;

using namespace IniParser;

GuiData::GuiData()
{
    s_GuiInfo = new PostParsingStruct;
}

GuiData::~GuiData()
{
    if(s_GuiInfo != 0) delete s_GuiInfo;
}

bool GuiData::load(string PathToGuiPack)
{
    ParserInfoFile prs;
    s_GuiInfo = prs.getParsedFromFile(PathToGuiPack + "gui.info");
    if(!s_GuiInfo) return false;
    map<string, map<string, string> > list_all_vars_fonts = s_GuiInfo->getMapVariables();
    map<string, string> list_all_fonts = list_all_vars_fonts["fonts"];
    map<string, string>::iterator iter1, iter2;
    int i = 0;
    for (iter1 = list_all_fonts.begin(), iter2 = list_all_fonts.end(); iter1 != iter2;)
    {
        if(iter1->second != "" && s_Fonts.find(iter1->second) == s_Fonts.end())
        {
            if(!s_Fonts[iter1->first].loadFromFile(PathToGuiPack + iter1->second)) cout << "Error loading " << iter1->first << " with file " << iter1->second << endl;
        }
        iter1++;
    }
    return true;
}

bool GuiData::isExists(string fnt_name)
{
    map<string, sf::Font >::iterator I = s_Fonts.find(fnt_name);
    if ( I == s_Fonts.end() ) return false;
    return true;
}

sf::Font GuiData::getSFMLFont(string fnt_name)
{
    sf::Font sfFont;
    if(isExists(fnt_name)) sfFont = s_Fonts[fnt_name];
    //else cout << "Error: font " << fnt_name << " does not exists!" << endl;
    return sfFont;
}

tgui::Font GuiData::getTGUIFont(string fnt_name)
{
    auto backendFont = make_shared<tgui::BackendFontSFML>();
    if(isExists(fnt_name)) backendFont->getInternalFont() = s_Fonts[fnt_name];
    else
    {
        //cout << "Error: font " << fnt_name << " does not exists!" << endl;
        return tgui::Font();
    }
    return tgui::Font(backendFont, "");
}
