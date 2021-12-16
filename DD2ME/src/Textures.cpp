#include <fstream>

#include "Textures.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

#include "Game.h"
#include "Defines.h"

using namespace IniParser;

using namespace std;

using namespace sf;

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
    map<int, Texture*>::iterator iter__, iter2__;
    for (iter__ = s_Tiles.begin(), iter2__ = s_Tiles.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0) delete iter__->second;
        ++iter__;
    }
    map<int, map<int, Texture*> >::iterator iter_, iter2_;
    for (iter_ = s_DeathTiles.begin(), iter2_ = s_DeathTiles.end(); iter_ != iter2_;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<int, Sprite*> >::iterator iter___, iter2___;
    for (iter___ = s_CacheTiles.begin(), iter2___ = s_CacheTiles.end(); iter___ != iter2___;)
    {
        map<int, Sprite* >::iterator _iter_, _iter2_;
        for (_iter_ = iter___->second.begin(), _iter2_ = iter___->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0) delete _iter_->second;
            ++_iter_;
        }
        ++iter___;
    }
    for (iter___ = s_CacheDeathTiles.begin(), iter2___ = s_CacheDeathTiles.end(); iter___ != iter2___;)
    {
        map<int, Sprite* >::iterator _iter_, _iter2_;
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
    int numberoftilesets = atoi( s_TilesInfo->getValue("info", "numberoftilesets").c_str() );
    for(int i = 0; i < numberoftilesets; i++)
    {
        //s_Tiles[i] = new Bitmap(PathToTexturePack + "tiles_" + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp");
        s_Tiles[i] = new Texture;
        if(!s_Tiles[i] || !s_Tiles[i]->loadFromFile(PathToTexturePack + "tiles_" + WorkFunctions::ConvertFunctions::itos(i+1) + ".bmp")) return false;
    }
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    for(int i = 0; i < numberoftilesets; i++)
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
    for(int j = 0; j < numberoftilesets; j++)
        for(int i = 0; i < numberoftiles; i++)
        {
            if(s_MaskTiles[j][i] == DEATH)
            {
                if(!s_DeathTilesInfo[i]) s_DeathTilesInfo[i] = prs.getParsedFromFile(PathToTexturePack + "DeathTiles/" + WorkFunctions::ConvertFunctions::itos(j + 1) + "/" + WorkFunctions::ConvertFunctions::itos(i) + "/tile.info");
                if(!s_DeathTilesInfo[i]) return false;
                numberofdeathframes = atoi( s_DeathTilesInfo[i]->getValue("info", "numberofframes").c_str() );
                for(int q = 0; q < numberofdeathframes; q++)
                    if(!s_DeathTiles[i][q])
                    {
                        //s_DeathTiles[i][q] = new Bitmap(PathToTexturePack + "DeathTiles/" + WorkFunctions::ConvertFunctions::itos(j + 1) + "/" + WorkFunctions::ConvertFunctions::itos(i) + "/" + WorkFunctions::ConvertFunctions::itos(q+1) + ".bmp");
                        s_DeathTiles[i][q] = new Texture;
                        if(!s_DeathTiles[i][q] || !s_DeathTiles[i][q]->loadFromFile(PathToTexturePack + "DeathTiles/" + WorkFunctions::ConvertFunctions::itos(j + 1) + "/" + WorkFunctions::ConvertFunctions::itos(i) + "/" + WorkFunctions::ConvertFunctions::itos(q+1) + ".bmp")) return false;
                    }
            }
        }

    int numberofanimations = atoi( s_TilesInfo->getValue("info", "numberofanimations").c_str() );
    for(int i = 0; i < numberofanimations; i++)
    {
        int numberofframestile = atoi( s_TilesInfo->getValue("animation_" + WorkFunctions::ConvertFunctions::itos(i + 1), "numberofframes").c_str() );
        if(!numberofframestile) return false;
        TileAnimationInfo tmptai;
        for(int j = 0; j < numberofframestile; j++)
        {
            int tile = atoi( s_TilesInfo->getValue("animation_" + WorkFunctions::ConvertFunctions::itos(i + 1), "frame" + WorkFunctions::ConvertFunctions::itos(j + 1)).c_str() );
            int sleep = atoi( s_TilesInfo->getValue("animation_" + WorkFunctions::ConvertFunctions::itos(i + 1), "sleep" + WorkFunctions::ConvertFunctions::itos(j + 1)).c_str() );
            TileAnimationStepInfo tmptasi;
            tmptasi.s_TileID = tile;
            tmptasi.s_AnimationSleep = sleep;
            tmptai.s_TileFrames.push_back(tmptasi);
            tmptai.s_GeneralAnimationSteps += sleep;
        }
        s_TilesAnimationInfo[0][tmptai.s_TileFrames[0].s_TileID] = tmptai;
    }
    return true;
}

int Textures::getCurrentAnimationTileID(int tile, int x, int y)
{
    return getTileIDByFrame(tile, getFrame(tile, x, y));
}

int Textures::getTileIDByFrame(int tile, int frame)
{
    if(s_TilesAnimationInfo[0].find(tile) == s_TilesAnimationInfo[0].end() || s_GameClass->s_IniFile->getValue("video", "tileanimation") != "true") return tile;
    return s_TilesAnimationInfo[0][tile].s_TileFrames[frame].s_TileID;
}

int Textures::getFrame(int tile, int x, int y)
{
    if(s_TilesAnimationInfo[0].find(tile) == s_TilesAnimationInfo[0].end() || s_GameClass->s_IniFile->getValue("video", "tileanimation") != "true") return 0;
    string anim_frame_lock = s_GameClass->s_Data->s_Level->getTileParameter(x, y, "GS_animation_frame_lock");
    if(anim_frame_lock != "")
    {
        int ret_frame = atoi(anim_frame_lock.c_str());
        return ret_frame % s_TilesAnimationInfo[0][tile].s_TileFrames.size();
    }
    string animation_shift_str = s_GameClass->s_Data->s_Level->getTileParameter(x, y, "GS_animation_shift");
    int animation_shift = 0;
    if(animation_shift_str != "") animation_shift = atoi(animation_shift_str.c_str());
    int tmpframe = (s_GameClass->s_TileAnimationStep + animation_shift) % s_TilesAnimationInfo[0][tile].s_GeneralAnimationSteps;
    int tmpschframe = 0;
    int sch = 0;
    for(sch = 0; sch < s_TilesAnimationInfo[0][tile].s_TileFrames.size(); sch++)
    {
        tmpschframe += s_TilesAnimationInfo[0][tile].s_TileFrames[sch].s_AnimationSleep;
        if(tmpschframe > tmpframe) break;
    }
    return sch;
}

void Textures::drawTile(int tile, int x, int y, int x_tile, int y_tile)
{
    if(tile < 0) return;
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    if(tile >= numberoftiles) return;
    int frame = getFrame(tile, x_tile, y_tile);
    tile = getTileIDByFrame(tile, frame);
    if(tile >= numberoftiles) return;
    int NumberDrawTileX = tile % sizeXTiles;
    int NumberDrawTileY = ( tile - ( tile % sizeXTiles ) ) / sizeXTiles;
    int tileset = 0;
    if(s_CacheCreated == false)
    {
        //s_GameClass->s_Window->draw(Image(Bitmap( (*s_Tiles[tileset]), NumberDrawTileX*16, NumberDrawTileY*16, 16, 16), x, y));
        Sprite spr(*s_Tiles[tileset]);
        spr.setTextureRect(IntRect(NumberDrawTileX*16, NumberDrawTileY*16, 16, 16));
        spr.setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        //s_GameClass->s_Window->draw(Image((*s_CacheTiles[tileset][tile]), x, y));
        s_CacheTiles[tileset][tile]->setPosition(x, y);
        s_GameClass->s_RenderTexture->draw(*s_CacheTiles[tileset][tile]);
    }
}

bool Textures::createCache()
{
    cout<<"Creating textures cache..."<<endl;
    int sizeXTiles = atoi( s_TilesInfo->getValue("info", "sizeX").c_str() );
    int sizeYTiles = atoi( s_TilesInfo->getValue("info", "sizeY").c_str() );
    int numberoftilesets = atoi( s_TilesInfo->getValue("info", "numberoftilesets").c_str() );
    int numberoftiles = sizeXTiles * sizeYTiles;
    int NumberDrawTileX;
    int NumberDrawTileY;
    for(int j = 0; j < numberoftilesets; j++)
        for(int i = 0; i < numberoftiles; i++)
        {
            NumberDrawTileX = i % sizeXTiles;
            NumberDrawTileY = ( i - ( i % sizeXTiles ) ) / sizeXTiles;
            //s_CacheTiles[j][i] = new Bitmap( (*s_Tiles[j]), NumberDrawTileX*16, NumberDrawTileY*16, 16, 16 );
            s_CacheTiles[j][i] = new Sprite(*s_Tiles[j]);
            s_CacheTiles[j][i]->setTextureRect(IntRect(NumberDrawTileX*16, NumberDrawTileY*16, 16, 16));
        }

    map<int, map<int, Texture*> >::iterator iter;
    map<int, Texture*>::iterator iter1;
    for ( iter = s_DeathTiles.begin(); iter != s_DeathTiles.end(); iter++ )
        for( iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++ )
        {
            //s_CacheDeathTiles[iter->first][iter1->first] = new Bitmap( (*iter1->second), 0, 0, iter1->second->width(), iter1->second->height() );
            s_CacheDeathTiles[iter->first][iter1->first] = new Sprite(*iter1->second);
            s_CacheDeathTiles[iter->first][iter1->first]->setTextureRect(IntRect(0, 0, iter1->second->getSize().x, iter1->second->getSize().y));
        }

    s_CacheCreated = true;
    cout<<"Textures cache created."<<endl;
    return true;
}

void Textures::createMaskTransparent(int r, int g, int b)
{
    map<int, Texture*>::iterator iter__, iter2__;
    for (iter__ = s_Tiles.begin(), iter2__ = s_Tiles.end(); iter__ != iter2__;)
    {
        if(iter__->second != 0)
        {
            //iter__->second->create_mask(r, g, b);
            sf::Image tmp_img = iter__->second->copyToImage();
            tmp_img.createMaskFromColor(Color(r, g, b));
            iter__->second->loadFromImage(tmp_img);
        }
        ++iter__;
    }
    map<int, map<int, Texture*> >::iterator iter_, iter2_;
    for (iter_ = s_DeathTiles.begin(), iter2_ = s_DeathTiles.end(); iter_ != iter2_;)
    {
        map<int, Texture* >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            if(_iter_->second != 0)
            {
                //iter__->second->create_mask(r, g, b);
                sf::Image tmp_img = _iter_->second->copyToImage();
                tmp_img.createMaskFromColor(Color(r, g, b));
                _iter_->second->loadFromImage(tmp_img);
            }
            ++_iter_;
        }
        ++iter_;
    }
}
