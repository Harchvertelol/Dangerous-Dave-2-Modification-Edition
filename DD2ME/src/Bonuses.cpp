#include "Bonuses.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"

using namespace IniParser;

using namespace std;

using namespace sf;

Bonuses::Bonuses(Game* gameclass):
    s_GameClass(gameclass),
    s_GlobBonusesInfo(0),
    s_CacheCreated(false)
{
    //...
}

Bonuses::~Bonuses()
{
    if(s_GlobBonusesInfo != 0) delete s_GlobBonusesInfo;
    map<int, PostParsingStruct*>::iterator iter, iter2;
    for (iter = s_BonusesInfo.begin(), iter2 = s_BonusesInfo.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    deleteAllGDIObjects();
}

void Bonuses::deleteAllGDIObjects()
{
    map<int, map<int, Sprite* > >::iterator iter__, iter2__;
    for (iter__ = s_BonusesCache.begin(), iter2__ = s_BonusesCache.end(); iter__ != iter2__;)
    {
        map<int, Sprite* >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter__;
    }
    map<string, map<int, Sprite* > >::iterator iter___, iter2___;
    for (iter___ = s_PointsBitmaps.begin(), iter2___ = s_PointsBitmaps.end(); iter___ != iter2___;)
    {
        map<int, Sprite* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter___;
    }
    map<int, map<int, Texture* > >::iterator iter_, iter2_;
    for (iter_ = s_BonusesBitmaps.begin(), iter2_ = s_BonusesBitmaps.end(); iter_ != iter2_;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<string, map<int, Texture* > >::iterator iter____, iter2____;
    for (iter____ = s_PointsBitmapsTextures.begin(), iter2____ = s_PointsBitmapsTextures.end(); iter____ != iter2____;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter____->second.begin(), _iter2_ = iter____->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter____;
    }
}

bool Bonuses::load(string PathToBonusPack)
{
    int numberofframes;
    ParserInfoFile prs;
    s_GlobBonusesInfo = prs.getParsedFromFile(PathToBonusPack + "bonuses.dat");
    if(!s_GlobBonusesInfo) return false;
    int numberofbonuses = atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() );
    for(int i = 0; i < numberofbonuses; i++)
    {
        s_BonusesInfo[i] = prs.getParsedFromFile(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + ".info");
        if(!s_BonusesInfo[i]) return false;
        //s_BonusesBitmaps[i][0] = new Bitmap(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp");
        s_BonusesBitmaps[i][0] = new Texture;
        if(!s_BonusesBitmaps[i][0] || !s_BonusesBitmaps[i][0]->loadFromFile(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp")) return false;
        numberofframes = atoi ( s_BonusesInfo[i]->getValue("info", "numberofframes").c_str() );
        for(int j = 0; j < numberofframes; j++)
        {
            //s_BonusesBitmaps[i][j+1] = new Bitmap(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + "_" + WorkFunctions::ConvertFunctions::itos(j+1) + ".bmp");
            s_BonusesBitmaps[i][j+1] = new Texture;
            if(!s_BonusesBitmaps[i][j+1] || !s_BonusesBitmaps[i][j+1]->loadFromFile(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + "_" + WorkFunctions::ConvertFunctions::itos(j+1) + ".bmp")) return false;
            collisionAnalyze(i, j+1);
        }
        /*s_BonusesBitmaps[i][numberofframes + 1] = new Bitmap(PathToBonusPack + WorkFunctions::ConvertFunctions::itos(i+1) + "_p.bmp");
        collisionAnalyze(i, numberofframes + 1);*/
    }
    map<string, string> tmpmappoints = s_GlobBonusesInfo->getMapVariables()["points"];
    map<string, string>::iterator tmpiter1, tmpiter2;
    for (tmpiter1 = tmpmappoints.begin(), tmpiter2 = tmpmappoints.end(); tmpiter1 != tmpiter2;)
    {
        string name = tmpiter1->first;
        string value = tmpiter1->second;
        //s_PointsBitmaps[name][0] = new Bitmap(PathToBonusPack + value);
        sf::Image tmp_image;
        s_PointsBitmapsTextures[name][0] = new Texture;
        if(!s_PointsBitmapsTextures[name][0] || !tmp_image.loadFromFile(PathToBonusPack + value) || !s_PointsBitmapsTextures[name][0]->loadFromImage(tmp_image, IntRect(0, 0, tmp_image.getSize().x / 2, tmp_image.getSize().y))) return false;
        s_PointsBitmaps[name][0] = new Sprite(*s_PointsBitmapsTextures[name][0]);
        //collisionAnalyze(i, numberofframes + 1);
        //int xSize = s_PointsBitmaps[name][0]->width()/2;
        //int ySize = s_PointsBitmaps[name][0]->height();
        //Bitmap* tmpbmp = new Bitmap( (*s_PointsBitmaps[name][0]), 0, 0, xSize, ySize);
        //delete s_PointsBitmaps[name][0];
        //s_PointsBitmaps[name][0] = tmpbmp;
        ++tmpiter1;
    }
    return true;
}

void Bonuses::collisionAnalyze(int anim, int frame)
{
    bool first = true;
    int xSize = s_BonusesBitmaps[anim][frame]->getSize().x / 2;
    int ySize = s_BonusesBitmaps[anim][frame]->getSize().y;
    //Bitmap dopBitmap( (*s_BonusesBitmaps[anim][frame]), xSize, 0, xSize, ySize );
    sf::Image tmp_image = s_BonusesBitmaps[anim][frame]->copyToImage();
    for(int i = 0; i < ySize; i++)
        for(int j = xSize; j < 2 * xSize; j++)
        {
            //if(dopBitmap.get_pixel_r(j,i) == 252 && first == true)
            if(tmp_image.getPixel(j, i).r == 252 && first == true)
            {
                s_Collisions[anim][frame].s_XL = j - xSize;
                s_Collisions[anim][frame].s_YL = i;
                first = false;
            }
            //else if(dopBitmap.get_pixel_r(j,i) == 252 && first == false)
            else if(tmp_image.getPixel(j, i).r == 252 && first == false)
            {
                s_Collisions[anim][frame].s_XR = j - xSize;
                s_Collisions[anim][frame].s_YR = i;
            }
        }
}

void Bonuses::drawBonus(int bonus, int x, int y, bool anim)
{
    if(bonus == 0 || bonus > atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() ) ) return;
    int frame;
    int numberofframes = atoi ( s_BonusesInfo[bonus-1]->getValue("info", "numberofframes").c_str() );
    if(anim == true && s_GameClass->s_IniFile->getValue("video", "bonusanimation") == "true") frame = s_GameClass->s_AnimationStep%numberofframes + 1;
    else frame = 1;
    /*if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_BonusesBitmaps[bonus-1][frame]), 0, 0, s_BonusesBitmaps[bonus-1][frame]->width()/2, s_BonusesBitmaps[bonus-1][frame]->height()), x, y));
    else s_GameClass->s_Window->draw( Image( (*s_BonusesCache[bonus-1][frame]), x, y));*/
    if(s_CacheCreated == false)
    {
        Sprite spr(*s_BonusesBitmaps[bonus-1][frame], IntRect(0, 0, s_BonusesBitmaps[bonus-1][frame]->getSize().x / 2, s_BonusesBitmaps[bonus-1][frame]->getSize().y));
        spr.setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        //s_GameClass->s_Window->draw( Image( (*s_BonusesCache[bonus-1][frame]), x, y));
        s_BonusesCache[bonus-1][frame]->setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(*s_BonusesCache[bonus-1][frame]);
    }
}

bool Bonuses::createCache()
{
    cout<<"Creating bonuses cache..."<<endl;
    int numberofframes, xSize, ySize;
    int numberofbonuses = atoi( s_GlobBonusesInfo->getValue("info", "numberofbonuses").c_str() );
    for(int i = 0; i < numberofbonuses; i++)
    {
        numberofframes = atoi ( s_BonusesInfo[i]->getValue("info", "numberofframes").c_str() );
        for(int j = 0; j < numberofframes; j++)
        {
            xSize = s_BonusesBitmaps[i][j+1]->getSize().x / 2;
            ySize = s_BonusesBitmaps[i][j+1]->getSize().y;
            //s_BonusesCache[i][j+1] = new Bitmap( (*s_BonusesBitmaps[i][j+1]), 0, 0, xSize, ySize);
            s_BonusesCache[i][j+1] = new Sprite;
            s_BonusesCache[i][j+1]->setTexture(*s_BonusesBitmaps[i][j+1]);
            s_BonusesCache[i][j+1]->setTextureRect(IntRect(0, 0, xSize, ySize));
        }
        /*xSize = s_BonusesBitmaps[i][numberofframes + 1]->width()/2;
        ySize = s_BonusesBitmaps[i][numberofframes + 1]->height();
        s_BonusesCache[i][numberofframes + 1] = new Bitmap( (*s_BonusesBitmaps[i][numberofframes + 1]), 0, 0, xSize, ySize);*/
    }
    s_CacheCreated = true;
    cout<<"Bonuses cache created."<<endl;
    return true;
}

void Bonuses::createMaskTransparent(int r, int g, int b)
{
    map<int, map<int, Texture* > >::iterator iter_, iter2_;
    for (iter_ = s_BonusesBitmaps.begin(), iter2_ = s_BonusesBitmaps.end(); iter_ != iter2_;)
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
    /*map<int, map<int, Bitmap* > >::iterator iter__, iter2__;
    for (iter__ = s_BonusesCache.begin(), iter2__ = s_BonusesCache.end(); iter__ != iter2__;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter__;
    }*/
    /*map<string, map<int, Bitmap* > >::iterator iter___, iter2___;
    for (iter___ = s_PointsBitmaps.begin(), iter2___ = s_PointsBitmaps.end(); iter___ != iter2___;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);
            ++_iter_;
        }
        ++iter___;
    }*/
    map<string, map<int, Texture* > >::iterator iter___, iter2___;
    for (iter___ = s_PointsBitmapsTextures.begin(), iter2___ = s_PointsBitmapsTextures.end(); iter___ != iter2___;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
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
        ++iter___;
    }
}
