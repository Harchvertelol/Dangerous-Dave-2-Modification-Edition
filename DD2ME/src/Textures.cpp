#include <fstream>

#include "Textures.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

Textures::Textures(Game* gameclass):
    s_GameClass(gameclass),
    s_TilesInfo(0),
    s_CacheCreated(false)
{
    //...
}

Textures::~Textures()
{
    if(s_TilesInfo != 0) delete s_TilesInfo;
    map<int, PostParsingStruct*>::iterator iter, iter2;
    for (iter = s_DeathTilesInfo.begin(), iter2 = s_DeathTilesInfo.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    deleteAllGDIObjects();
}

void Textures::deleteAllGDIObjects()
{
    map<int, Bitmap*>::iterator iter__, iter2__;
    for (iter__ = s_Tiles.begin(), iter2__ = s_Tiles.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0) delete iter__->second;
        ++iter__;
    }
    map<int, map<int, Bitmap*> >::iterator iter_, iter2_;
    for (iter_ = s_DeathTiles.begin(), iter2_ = s_DeathTiles.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<int, Bitmap*> >::iterator iter___, iter2___;
    for (iter___ = s_CacheTiles.begin(), iter2___ = s_CacheTiles.end(); iter___ != iter2___;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter___;
    }
    for (iter___ = s_CacheDeathTiles.begin(), iter2___ = s_CacheDeathTiles.end(); iter___ != iter2___;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter___;
    }
}

bool Textures::load(string PathToTexturePack)
{
    ParserInfoFile prs;
    s_TilesInfo = prs.getParsedFromFile(PathToTexturePack + "tiles.info");
    if(!s_TilesInfo) return false;
    int numberofframes = atoi( s_TilesInfo->getValue("info", "numberofframes").c_str() );
    for(int i = 0; i < numberofframes; i++) s_Tiles[i] = new Bitmap(PathToTexturePack + "tiles_" + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp");
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    for(int i = 0; i < numberofframes; i++)
    {
        ifstream mask( (PathToTexturePack + "tiles_" + WorkFunctions::ConvertFunctions::itos(i+1) + ".mask").c_str() );
        if(!mask) return false;
        for(int j = 0; mask && j < numberoftiles; j++)
        {
            mask >> s_MaskTiles[i][j];
            if(!mask && j < numberoftiles) return false;
        }
        mask.close();
    }
    int numberofdeathframes;
    for(int j = 0; j < numberofframes; j++)
        for(int i = 0; i < numberoftiles; i++)
        {
            if(s_MaskTiles[j][i] == DEATH)
            {
                if(!s_DeathTilesInfo[i]) s_DeathTilesInfo[i] = prs.getParsedFromFile(PathToTexturePack + "DeathTiles/" + WorkFunctions::ConvertFunctions::itos(i) + "/tile.info");
                if(!s_DeathTilesInfo[i]) return false;
                numberofdeathframes = atoi( s_DeathTilesInfo[i]->getValue("info","numberofframes").c_str() );
                for(int q = 0; q < numberofdeathframes; q++)
                    if(!s_DeathTiles[i][q]) s_DeathTiles[i][q] = new Bitmap(PathToTexturePack + "DeathTiles/" + WorkFunctions::ConvertFunctions::itos(i) + "/" + WorkFunctions::ConvertFunctions::itos(q+1) + ".bmp");
            }
        }
    return true;
}

int Textures::getFrame()
{
    int numberofframes = atoi( s_TilesInfo->getValue("info", "numberofframes").c_str() );
    int frame;
    if(s_GameClass->s_IniFile->getValue("video", "tileanimation") == "true") frame = s_GameClass->s_AnimationStep%numberofframes;
    else frame = 0;
    return frame;
}

void Textures::drawTile(int tile, int x, int y)
{
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    if(tile >= numberoftiles) return;
    int NumberDrawTileX = tile % sizeXTiles;
    int NumberDrawTileY = ( tile - ( tile % sizeXTiles ) ) / sizeXTiles;
    int frame = getFrame();
    if(s_CacheCreated == false) s_GameClass->s_Window->draw(Image(Bitmap( (*s_Tiles[frame]), NumberDrawTileX*16, NumberDrawTileY*16, 16, 16), x, y));
    else
    {
        /*s_CacheTiles[frame][tile]->toX = x;
        s_CacheTiles[frame][tile]->toY = y;
        s_GameClass->s_Window->draw( (*s_CacheTiles[frame][tile]) );*/
        s_GameClass->s_Window->draw(Image((*s_CacheTiles[frame][tile]), x, y));
    }
}

bool Textures::createCache()
{
    cout<<"Creating textures cache..."<<endl;
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberofframes = atoi( s_TilesInfo->getValue("info", "numberofframes").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    int NumberDrawTileX;
    int NumberDrawTileY;
    for(int j = 0; j < numberofframes; j++)
        for(int i = 0; i < numberoftiles; i++)
        {
            NumberDrawTileX = i % sizeXTiles;
            NumberDrawTileY = ( i - ( i % sizeXTiles ) ) / sizeXTiles;
            s_CacheTiles[j][i] = new Bitmap( (*s_Tiles[j]), NumberDrawTileX*16, NumberDrawTileY*16, 16, 16 );
        }

    map<int, map<int, Bitmap*> >::iterator iter;
    map<int, Bitmap*>::iterator iter1;
    for ( iter = s_DeathTiles.begin(); iter != s_DeathTiles.end(); iter++ )
        for( iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++ ) s_CacheDeathTiles[iter->first][iter1->first] = new Bitmap( (*iter1->second), 0, 0, iter1->second->width(), iter1->second->height() );

    s_CacheCreated = true;
    cout<<"Textures cache created."<<endl;
    return true;
}

void Textures::createMaskTransparent(int r, int g, int b)
{
    map<int, Bitmap*>::iterator iter__, iter2__;
    for (iter__ = s_Tiles.begin(), iter2__ = s_Tiles.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0) iter__->second->create_mask(r, g, b);
        ++iter__;
    }
    map<int, map<int, Bitmap*> >::iterator iter_, iter2_;
    for (iter_ = s_DeathTiles.begin(), iter2_ = s_DeathTiles.end(); iter_ != iter2_;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);;
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<int, Bitmap*> >::iterator iter___, iter2___;
    for (iter___ = s_CacheTiles.begin(), iter2___ = s_CacheTiles.end(); iter___ != iter2___;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);;
            ++_iter_;
        }
        ++iter___;
    }
    for (iter___ = s_CacheDeathTiles.begin(), iter2___ = s_CacheDeathTiles.end(); iter___ != iter2___;)
    {
        map<int, Bitmap* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) _iter_->second->create_mask(r, g, b);;
            ++_iter_;
        }
        ++iter___;
    }
}
