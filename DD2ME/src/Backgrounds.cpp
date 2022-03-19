#include <fstream>

#include "Backgrounds.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

Backgrounds::Backgrounds(Game* gameclass):
    s_GameClass(gameclass),
    s_BackgroundsInfo(0),
    s_CacheCreated(false)
{
    //...
}

Backgrounds::~Backgrounds()
{
    if(s_BackgroundsInfo != 0) delete s_BackgroundsInfo;
}

void Backgrounds::deleteAllGDIObjects()
{
    s_BackgroundsList.clear();
    s_CacheBackgroundsList.clear();
}

bool Backgrounds::load(string PathToBackgroundsPack)
{
    ParserInfoFile prs;
    s_BackgroundsInfo = prs.getParsedFromFile(PathToBackgroundsPack + "backgrounds.info");
    if(!s_BackgroundsInfo) return false;
    map<string, map<string, string> > list_all_vars_backgrounds = s_BackgroundsInfo->getMapVariables();
    map<string, string> list_all_backgrounds = list_all_vars_backgrounds["backgrounds"];
    map<string, string>::iterator iter1, iter2;
    for (iter1 = list_all_backgrounds.begin(), iter2 = list_all_backgrounds.end(); iter1 != iter2;)
    {
        if(iter1->second != "" && s_BackgroundsList.find(iter1->second) == s_BackgroundsList.end())
        {
            if(!s_BackgroundsList[iter1->first].loadFromFile(PathToBackgroundsPack + iter1->second))
            {
                cout << "Error loading " << iter1->first << " with file " << iter1->second << endl;
                return false;
            }
        }
        iter1++;
    }
    return true;
}

bool Backgrounds::createCache()
{
    cout<<"Creating backgrounds cache..."<<endl;
    map<string, sf::Texture>::iterator iter1, iter2;
    for(iter1 = s_BackgroundsList.begin(), iter2 = s_BackgroundsList.end(); iter1 != iter2;)
    {
        s_CacheBackgroundsList[iter1->first] = sf::Sprite(iter1->second);
        iter1++;
    }
    cout<<"Backgrounds cache created."<<endl;
    s_CacheCreated = true;
    return true;
}

void Backgrounds::draw(string name, int x, int y, float scalex, float scaley)
{
    if(!isExists(name))
    {
        cout << "Error: background with name \"" << name << "\" does not exists!" << endl;
        return;
    }
    if(s_CacheCreated == false)
    {
        sf::Sprite spr(s_BackgroundsList[name]);
        spr.setPosition(x, y);
        spr.setScale(scalex, scaley);
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        s_CacheBackgroundsList[name].setPosition(x, y);
        s_CacheBackgroundsList[name].setScale(scalex, scaley);
        s_GameClass->s_RenderTexture->draw(s_CacheBackgroundsList[name]);
    }
}

void Backgrounds::createMaskTransparent(int r, int g, int b)
{
    map<string, sf::Texture>::iterator iter__, iter2__;
    for(iter__ = s_BackgroundsList.begin(), iter2__ = s_BackgroundsList.end(); iter__ != iter2__;)
    {
        sf::Image tmp_img = iter__->second.copyToImage();
        tmp_img.createMaskFromColor(sf::Color(r, g, b));
        iter__->second.loadFromImage(tmp_img);
        ++iter__;
    }
}

int Backgrounds::getSizeBackgroundX(string name)
{
    if(!isExists(name)) return -1;
    return s_BackgroundsList[name].getSize().x;
}

int Backgrounds::getSizeBackgroundY(string name)
{
    if(!isExists(name)) return -1;
    return s_BackgroundsList[name].getSize().y;
}

bool Backgrounds::isExists(string name)
{
    if(s_BackgroundsList.find(name) == s_BackgroundsList.end()) return false;
    return true;
}
