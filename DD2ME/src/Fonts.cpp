#include "Fonts.h"

#include <iostream>

#include "IniParser/ParserInfoFile.h"

using namespace std;

using namespace sf;

using namespace IniParser;

Fonts::Fonts()
{
    s_FontsInfo = new PostParsingStruct;
}

Fonts::~Fonts()
{
    if(s_FontsInfo != 0) delete s_FontsInfo;
}

bool Fonts::load(string PathToFontsPack)
{
    ParserInfoFile prs;
    s_FontsInfo = prs.getParsedFromFile(PathToFontsPack + "fonts.info");
    if(!s_FontsInfo) return false;
    map<string, map<string, string> > list_all_vars_fonts = s_FontsInfo->getMapVariables();
    map<string, string> list_all_fonts = list_all_vars_fonts["fonts"];
    map<string, string>::iterator iter1, iter2;
    int i = 0;
    for (iter1 = list_all_fonts.begin(), iter2 = list_all_fonts.end(); iter1 != iter2;)
    {
        if(iter1->second != "" && s_Fonts.find(iter1->second) == s_Fonts.end())
        {
            if(!s_Fonts[iter1->first].loadFromFile(PathToFontsPack + iter1->second)) cout << "Error loading " << iter1->first << " with file " << iter1->second << endl;
        }
        iter1++;
    }
    return true;
}

Font Fonts::getFont(string fnt_name)
{
    return s_Fonts[fnt_name];
}
