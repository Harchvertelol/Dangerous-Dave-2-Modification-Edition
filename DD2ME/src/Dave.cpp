#include "Dave.h"

#include "Game.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

using namespace WorkFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

using namespace std;

using namespace sf;

Dave::Dave(Game* gameclass):
    s_GameClass(gameclass),
    s_DaveInfo(0),
    s_CacheCreated(false)
{
    //...
}

Dave::~Dave()
{
    if(s_DaveInfo != 0) delete s_DaveInfo;
    deleteAllGDIObjects();
}

void Dave::deleteAllGDIObjects()
{
    map<string, map<int, Texture* > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<string, map<int, Sprite* > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<int, Sprite* >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter__;
    }
}

bool Dave::load(string PathToDavePack)
{
    string state = "";
    int numberofframes = 0;
    ParserInfoFile prs;
    s_DaveInfo = prs.getParsedFromFile(PathToDavePack + "dave.dat");
    if(!s_DaveInfo) return false;
    map<string, string>::iterator iter;
    for ( iter = s_DaveInfo->getMapVariables()["info"].begin(); iter != s_DaveInfo->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            //s_Bitmaps[state][i] = new Bitmap(PathToDavePack + state + "_" + itos(i+1) + ".bmp");
            s_Bitmaps[state][i] = new Texture;
            if(!s_Bitmaps[state][i] || !s_Bitmaps[state][i]->loadFromFile(PathToDavePack + state + "_" + itos(i+1) + ".bmp")) return false;
            collisionAnalyze(state,i);
        }
    }
    for(int i = 0; i < 9; i++)
    {
        //s_Bitmaps["bandolier"][i] = new Bitmap(PathToDavePack + "bandolier_" + itos(i) + ".bmp");
        s_Bitmaps["bandolier"][i] = new Texture;
        if(!s_Bitmaps["bandolier"][i] || !s_Bitmaps["bandolier"][i]->loadFromFile(PathToDavePack + "bandolier_" + itos(i) + ".bmp")) return false;
    }
    return true;
}

bool Dave::createCache()
{
    cout<<"Creating dave cache..."<<endl;

    int numberofframes, xSize, ySize;
    string state;
    map<string, string>::iterator iter;
    for ( iter = s_DaveInfo->getMapVariables()["info"].begin(); iter != s_DaveInfo->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            xSize = s_Bitmaps[state][i]->getSize().x / 2;
            ySize = s_Bitmaps[state][i]->getSize().y;
            //s_CacheImages[state][i] = new Bitmap( *s_Bitmaps[state][i], 0, 0, xSize, ySize);
            s_CacheImages[state][i] = new Sprite( *s_Bitmaps[state][i], IntRect(0, 0, xSize, ySize));
        }
    }
    for(int i = 0; i < 9; i++)
    {
        xSize = s_Bitmaps["bandolier"][i]->getSize().x / 2 - 3;
        ySize = s_Bitmaps["bandolier"][i]->getSize().y;
        //s_CacheImages["bandolier"][i] = new Bitmap( *s_Bitmaps["bandolier"][i], 0, 0, xSize, ySize);
        s_CacheImages["bandolier"][i] = new Sprite( *s_Bitmaps["bandolier"][i], IntRect(0, 0, xSize, ySize));
    }
    s_CacheCreated = true;
    cout<<"Dave cache created."<<endl;
    return true;
}

void Dave::drawDave(string anim, int frame, int x, int y)
{
    if(s_CacheCreated == false)
    {
        //s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmaps[anim][frame]), 0, 0, s_Bitmaps[anim][frame]->width()/2, s_Bitmaps[anim][frame]->height()), x, y));
        Sprite spr;
        spr.setTexture(*s_Bitmaps[anim][frame]);
        spr.setTextureRect(IntRect(0, 0, s_Bitmaps[anim][frame]->getSize().x / 2, s_Bitmaps[anim][frame]->getSize().y));
        spr.setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        //s_GameClass->s_Window->draw( Image( (*s_CacheImages[anim][frame]), x, y) );
        s_CacheImages[anim][frame]->setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(*s_CacheImages[anim][frame]);
    }
}

void Dave::drawBandolier(int frame, int x, int y)
{
    drawDave("bandolier", frame, x, y);
}

void Dave::collisionAnalyze(string anim, int frame)
{
    bool first = true;
    int xSize = s_Bitmaps[anim][frame]->getSize().x / 2;
    int ySize = s_Bitmaps[anim][frame]->getSize().y;
    //Bitmap dopBitmap( (*s_Bitmaps[anim][frame]), xSize, 0, xSize, ySize );
    sf::Image tmp_image = s_Bitmaps[anim][frame]->copyToImage();
    for(int i = 0; i < ySize; i++)
        for(int j = xSize; j < 2 * xSize; j++)
        {
            if(tmp_image.getPixel(j, i).r == 252 && first == true)
            {
                s_Collisions[anim][frame].s_XL = j - xSize;
                s_Collisions[anim][frame].s_YL = i;
                first = false;
            }
            else if(tmp_image.getPixel(j, i).r == 252 && first == false)
            {
                s_Collisions[anim][frame].s_XR = j - xSize;
                s_Collisions[anim][frame].s_YR = i;
            }
        }
}

void Dave::createMaskTransparent(int r, int g, int b)
{
    map<string, map<int, Texture* > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0)
            {
                //_iter_->second->create_mask(r, g, b);
                sf::Image tmp_img = _iter_->second->copyToImage();
                tmp_img.createMaskFromColor(Color(r, g, b));
                _iter_->second->loadFromImage(tmp_img);
            }
            ++_iter_;
        }
        ++iter_;
    }
}
