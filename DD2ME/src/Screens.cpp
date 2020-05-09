#include "Screens.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"

using namespace IniParser;

Screens::Screens(Game* gameclass):
    s_GameClass(gameclass),
    s_ScreensInfo(0),
    s_ChangeLevelInfo(0),
    s_MainScreenInfo(0),
    s_CacheCreated(false)
{
    //...
}

Screens::~Screens()
{
    if(s_ScreensInfo != 0) delete s_ScreensInfo;
    if(s_ChangeLevelInfo != 0) delete s_ChangeLevelInfo;
    if(s_MainScreenInfo != 0) delete s_MainScreenInfo;
    deleteAllGDIObjects();
}

void Screens::deleteAllGDIObjects()
{
    map<string, Bitmap*>::iterator iter, iter2;
    for (iter = s_StandardScreens.begin(), iter2 = s_StandardScreens.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    map<string, map<int, Bitmap*> >::iterator iter_, iter2_;
    for (iter_ = s_MCScreens.begin(), iter2_ = s_MCScreens.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<string, Bitmap*>::iterator iter__, iter2__;
    for (iter__ = s_CacheStandardScreens.begin(), iter2__ = s_CacheStandardScreens.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0) delete iter__->second;
        ++iter__;
    }
    map<string, map<int, Bitmap*> >::iterator iter___, iter2___;
    for (iter___ = s_CacheMCScreens.begin(), iter2___ = s_CacheMCScreens.end(); iter___ != iter2___;)
    {
        map<int, Bitmap*>::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter___;
    }
}

bool Screens::load(string PathToScreenPack)
{
    int numberofscreens;
    ParserInfoFile prs;
    s_ScreensInfo = prs.getParsedFromFile(PathToScreenPack + "screens.dat");
    if(!s_ScreensInfo) return false;
    s_ChangeLevelInfo = prs.getParsedFromFile(PathToScreenPack + "changelevel.info");
    if(!s_ChangeLevelInfo) return false;
    s_MainScreenInfo = prs.getParsedFromFile(PathToScreenPack + "mainscreen.info");
    if(!s_MainScreenInfo) return false;
    string flylogo = s_ScreensInfo->getValue("info","flylogo");
    string flybackground = s_ScreensInfo->getValue("info","flybackground");
    string endscreen = s_ScreensInfo->getValue("info","endscreen");
    string changelevelprefix = s_ScreensInfo->getValue("info","changelevelprefix");
    string mainscreenprefix = s_ScreensInfo->getValue("info","mainscreenprefix");
    s_StandardScreens["flylogo"] = new Bitmap(PathToScreenPack + flylogo + ".bmp");
    s_StandardScreens["flybackground"] = new Bitmap(PathToScreenPack + flybackground + ".bmp");
    s_StandardScreens["endscreen"] = new Bitmap(PathToScreenPack + endscreen + ".bmp");
    numberofscreens = atoi( s_MainScreenInfo->getValue("info", "numberofscreens").c_str() );
    for(int i = 0; i < numberofscreens; i++) s_MCScreens["mainscreen"][i] = new Bitmap(PathToScreenPack + mainscreenprefix + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp");
    numberofscreens = atoi( s_ChangeLevelInfo->getValue("info", "numberofscreens").c_str() );
    for(int i = 0; i < numberofscreens; i++) s_MCScreens["changelevelscreen"][i] = new Bitmap(PathToScreenPack + changelevelprefix + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp");
    return true;
}

bool Screens::createCache()
{
    cout<<"Creating screens cache..."<<endl;
    int numberofscreens;
    s_CacheStandardScreens["flylogo"] = new Bitmap(*s_StandardScreens["flylogo"]);
    s_CacheStandardScreens["flybackground"] = new Bitmap(*s_StandardScreens["flybackground"]);
    s_CacheStandardScreens["endscreen"] = new Bitmap(*s_StandardScreens["endscreen"]);
    numberofscreens = atoi( s_MainScreenInfo->getValue("info", "numberofscreens").c_str() );
    for(int i = 0; i < numberofscreens; i++) s_CacheMCScreens["mainscreen"][i] = new Bitmap(*s_MCScreens["mainscreen"][i]);
    numberofscreens = atoi( s_ChangeLevelInfo->getValue("info", "numberofscreens").c_str() );
    for(int i = 0; i < numberofscreens; i++) s_CacheMCScreens["changelevelscreen"][i] = new Bitmap(*s_MCScreens["changelevelscreen"][i]);
    s_CacheCreated = true;
    cout<<"Screens cache created."<<endl;
    return true;
}

void Screens::drawScreen(string name, int x, int y, int number)
{
    if(name == "flylogo" || name == "flybackground" || name == "endscreen")
    {
        if(s_CacheCreated == false)
        {
            if(name == "flylogo") s_GameClass->s_Window->draw(Image( *s_StandardScreens[name], x, y ));
            else s_GameClass->s_Window->draw(Image(*s_StandardScreens[name], x, y ));
        }
        else
        {
            if(name == "flylogo") s_GameClass->s_Window->draw(Image(*s_CacheStandardScreens[name], x, y));
            else s_GameClass->s_Window->draw(Image(*s_CacheStandardScreens[name], x, y));
        }
    }
    if(name == "mainscreen" || name == "changelevelscreen")
    {
        if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image( *s_MCScreens[name][number], x, y ));
        else s_GameClass->s_Window->draw(Image(*s_CacheMCScreens[name][number], x, y));
    }
}

void Screens::createMaskTransparent(int r, int g, int b)
{
    map<string, Bitmap*>::iterator iter, iter2;
    for (iter = s_StandardScreens.begin(), iter2 = s_StandardScreens.end(); iter != iter2;)
    {
        if(iter->second != 0) iter->second->create_mask(r, g, b);
        ++iter;
    }
    map<string, map<int, Bitmap*> >::iterator iter_, iter2_;
    for (iter_ = s_MCScreens.begin(), iter2_ = s_MCScreens.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter_;
    }
    map<string, Bitmap*>::iterator iter__, iter2__;
    for (iter__ = s_CacheStandardScreens.begin(), iter2__ = s_CacheStandardScreens.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0) iter__->second->create_mask(r, g, b);
        ++iter__;
    }
    map<string, map<int, Bitmap*> >::iterator iter___, iter2___;
    for (iter___ = s_CacheMCScreens.begin(), iter2___ = s_CacheMCScreens.end(); iter___ != iter2___;)
    {
        map<int, Bitmap*>::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter___;
    }
}
