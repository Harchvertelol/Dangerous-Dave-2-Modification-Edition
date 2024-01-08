#include "Monsters.h"

#include "Game.h"

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"
#include "Defines.h"

using namespace WorkFunctions;
using namespace MathFunctions;
using namespace GameFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

using namespace std;

using namespace sf;

Monsters::Monsters(Game* gameclass):
    s_GameClass(gameclass),
    s_GlobMonstersInfo(0),
    s_CacheCreated(false)
{
    //...
}

Monsters::~Monsters()
{
    if(s_GlobMonstersInfo != 0) delete s_GlobMonstersInfo;
    map<int, PostParsingStruct*>::iterator iter, iter2;
    for (iter = s_MonstersInfo.begin(), iter2 = s_MonstersInfo.end(); iter != iter2;)
    {
        if(iter->second != 0) delete iter->second;
        ++iter;
    }
    deleteAllGDIObjects();
}

void Monsters::deleteAllGDIObjects()
{
    map<int, map<string, map<int, Texture*> > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<string, map<int, Texture*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            map<int, Texture*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) delete _iter__->second;
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter_;
    }
    map<int, map<string, map<int, Sprite*> > >::iterator iter__, iter2__;
    for (iter__ = s_CacheImages.begin(), iter2__ = s_CacheImages.end(); iter__ != iter2__;)
    {
        map<string, map<int, Sprite*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter__->second.begin(), _iter2_ = iter__->second.end(); _iter_ != _iter2_;)
        {
            map<int, Sprite*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0) delete _iter__->second;
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter__;
    }
}

bool Monsters::load(string PathToMonsterPack)
{
    int numberofmonsters;
    ParserInfoFile prs;
    s_GlobMonstersInfo = prs.getParsedFromFile(PathToMonsterPack + "monsters.dat");
    if(!s_GlobMonstersInfo) return false;
    numberofmonsters = atoi( s_GlobMonstersInfo->getValue("info","numberofmonsters").c_str() );
    for(int i = 0; i < numberofmonsters; i++)
        if( !loadMonster(PathToMonsterPack + itos(i+1) + "/", i ) ) return false;
    return true;
}

bool Monsters::loadMonster(string PathToMonster, int number)
{
    int numberofframes;
    string state;
    ParserInfoFile prs;
    s_MonstersInfo[number] = prs.getParsedFromFile(PathToMonster + "monster.info");
    if(!s_MonstersInfo[number]) return false;
    string tmp_ext = "." + s_MonstersInfo[number]->getValue("other", "extensions");
    map<string, string>::iterator iter;
    for ( iter = s_MonstersInfo[number]->getMapVariables()["info"].begin(); iter != s_MonstersInfo[number]->getMapVariables()["info"].end(); iter++ )
    {
        state = iter->first.substr(14);
        numberofframes = atoi( iter->second.c_str() );
        for(int i = 0; i < numberofframes; i++)
        {
            //s_Bitmaps[number][state][i] = new Bitmap(PathToMonster + state + "_" + itos(i+1) + ".bmp");
            //if(s_Bitmaps[number][state][i]->empty()) return false;
            s_Bitmaps[number][state][i] = new Texture;
            if(!s_Bitmaps[number][state][i] || !s_Bitmaps[number][state][i]->loadFromFile(PathToMonster + state + "_" + itos(i+1) + tmp_ext)) return false;
            collisionAnalyze(number, state, i);
        }
    }
    numberofframes = atoi( s_MonstersInfo[number]->getValue("other", "numberofframesdeathtiles").c_str() );
    for(int i = 0; i < numberofframes; i++)
    {
        //s_Bitmaps[number]["deathtiles"][i] = new Bitmap(PathToMonster + "DeathTiles/" + itos(i+1) + ".bmp");
        //if(s_Bitmaps[number]["deathtiles"][i]->empty()) return false;
        s_Bitmaps[number]["deathtiles"][i] = new Texture;
        if(!s_Bitmaps[number]["deathtiles"][i] || !s_Bitmaps[number]["deathtiles"][i]->loadFromFile(PathToMonster + "DeathTiles/" + itos(i+1) + tmp_ext)) return false;
    }
    return true;
}

bool Monsters::createCache()
{
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Creating monsters cache...");
    int numberofframes, xSize, ySize;
    string state;
    int numberofmonsters = atoi( s_GlobMonstersInfo->getValue("info", "numberofmonsters").c_str() );
    for(int number = 0; number < numberofmonsters; number++)
    {
        map<string, string>::iterator iter;
        for ( iter = s_MonstersInfo[number]->getMapVariables()["info"].begin(); iter != s_MonstersInfo[number]->getMapVariables()["info"].end(); iter++ )
        {
            state = iter->first.substr(14);
            numberofframes = atoi( iter->second.c_str() );
            for(int i = 0; i < numberofframes; i++)
            {
                xSize = s_Bitmaps[number][state][i]->getSize().x / 2;
                ySize = s_Bitmaps[number][state][i]->getSize().y;
                //s_CacheImages[number][state][i] = new Bitmap( *s_Bitmaps[number][state][i], 0, 0, xSize, ySize);
                s_CacheImages[number][state][i] = new Sprite(*s_Bitmaps[number][state][i]);
                s_CacheImages[number][state][i]->setTextureRect(IntRect(0, 0, xSize, ySize));
            }
            numberofframes = atoi( s_MonstersInfo[number]->getValue("other","numberofframesdeathtiles").c_str() );
            for(int i = 0; i < numberofframes; i++)
            {
                xSize = s_Bitmaps[number]["deathtiles"][i]->getSize().x;
                ySize = s_Bitmaps[number]["deathtiles"][i]->getSize().y;
                //s_CacheImages[number]["deathtiles"][i] = new Bitmap( *s_Bitmaps[number]["deathtiles"][i], 0, 0, xSize, ySize);
                s_CacheImages[number]["deathtiles"][i] = new Sprite(*s_Bitmaps[number]["deathtiles"][i]);
                s_CacheImages[number]["deathtiles"][i]->setTextureRect(IntRect(0, 0, xSize, ySize));
            }
        }
    }
    s_CacheCreated = true;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Monsters cache was created.");
    return true;
}

void Monsters::collisionAnalyze(int number, string state, int frame)
{
    bool first = true;
    int xSize = s_Bitmaps[number][state][frame]->getSize().x / 2;
    int ySize = s_Bitmaps[number][state][frame]->getSize().y;
    //Bitmap dopBitmap( (*s_Bitmaps[number][state][frame]), xSize, 0, xSize, ySize );
    sf::Image tmp_image = s_Bitmaps[number][state][frame]->copyToImage();
    for(int i = 0; i < ySize; i++)
        for(int j = xSize; j < 2 * xSize; j++)
        {
            if(tmp_image.getPixel(j, i).r == 252 && first == true)
            {
                s_Collisions[number][state][frame].s_XL = j - xSize;
                s_Collisions[number][state][frame].s_YL = i;
                first = false;
            }
            else if(tmp_image.getPixel(j, i).r == 252 && first == false)
            {
                s_Collisions[number][state][frame].s_XR = j - xSize;
                s_Collisions[number][state][frame].s_YR = i;
            }
        }
}

void Monsters::drawMonster(int number, string anim, int frame, int x, int y, bool is_highlighted)
{
    sf::Color spr_color = sf::Color(255, 255, 255, 255);
    string str_highlite_mode = "";
    if(is_highlighted)
    {
        map<int, int> tmp_mas;
        string str_highlighting = s_GameClass->s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "highlightingonhit");
        if(s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->isExists("other", "highlightingonhit")) str_highlighting = s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->getValue("other", "highlightingonhit");

        str_highlite_mode = s_GameClass->s_Data->s_Monsters->s_GlobMonstersInfo->getValue("draw", "highlightingonhitmode");
        if(s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->isExists("other", "highlightingonhitmode")) str_highlite_mode = s_GameClass->s_Data->s_Monsters->s_MonstersInfo[number - 1]->getValue("other", "highlightingonhitmode");

        int res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, str_highlighting, ";");
        if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with parameters for highlighting monster: highlightingonhit");
        else spr_color = tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]);
    }
    if(s_CacheCreated == false)
    {
        //s_GameClass->s_Window->draw(Image(Bitmap( (*s_Bitmaps[number - 1][anim][frame]), 0, 0, s_Bitmaps[number - 1][anim][frame]->width()/2, s_Bitmaps[number - 1][anim][frame]->height()), x, y));
        Sprite spr(*s_Bitmaps[number - 1][anim][frame]);
        spr.setTextureRect(IntRect(0, 0, s_Bitmaps[number - 1][anim][frame]->getSize().x / 2, s_Bitmaps[number - 1][anim][frame]->getSize().y));
        spr.setPosition(x, y);
        if(is_highlighted && str_highlite_mode == "fill")
        {
            if (!sf::Shader::isAvailable()) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Shaders not avaliable!");
            else
            {
                const std::string highlight_shader_code = \
                "uniform sampler2D texture;" \
                "uniform vec4 highlightcolor;" \
                "void main()" \
                "{" \
                "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);" \
                "if(pixel.a == 1.0){" \
                "gl_FragColor = vec4(highlightcolor.r, highlightcolor.g, highlightcolor.b, 1.0);" \
                "} else {" \
                "discard;" \
                "}" \
                "}";
                Shader highlight_shader;
                if(!highlight_shader.loadFromMemory(highlight_shader_code, Shader::Fragment)) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Highlight shader load failed!");
                else
                {
                    highlight_shader.setUniform("texture", Shader::CurrentTexture);
                    highlight_shader.setUniform("highlightcolor", Glsl::Vec4(spr_color));
                    s_GameClass->s_RenderTexture->draw(spr, &highlight_shader);
                    return;
                }
            }
        }
        if(str_highlite_mode == "multiply") spr.setColor(spr_color);
        else spr.setColor(Color(255, 255, 255, 255));
        s_GameClass->s_RenderTexture->draw(spr);
    }
    else
    {
        //s_GameClass->s_Window->draw( Image( (*s_CacheImages[number - 1][anim][frame]), x, y) );
        s_CacheImages[number - 1][anim][frame]->setPosition(x, y);
        if(is_highlighted && str_highlite_mode == "fill")
        {
            if (!sf::Shader::isAvailable()) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Shaders not avaliable!");
            else
            {
                const std::string highlight_shader_code = \
                "uniform sampler2D texture;" \
                "uniform vec4 highlightcolor;" \
                "void main()" \
                "{" \
                "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);" \
                "if(pixel.a == 1.0){" \
                "gl_FragColor = vec4(highlightcolor.r, highlightcolor.g, highlightcolor.b, 1.0);" \
                "} else {" \
                "discard;" \
                "}" \
                "}";
                Shader highlight_shader;
                if(!highlight_shader.loadFromMemory(highlight_shader_code, Shader::Fragment)) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Highlight shader load failed!");
                else
                {
                    highlight_shader.setUniform("texture", Shader::CurrentTexture);
                    highlight_shader.setUniform("highlightcolor", Glsl::Vec4(spr_color));
                    s_GameClass->s_RenderTexture->draw(*s_CacheImages[number - 1][anim][frame], &highlight_shader);
                    return;
                }
            }
        }
        if(str_highlite_mode == "multiply") s_CacheImages[number - 1][anim][frame]->setColor(spr_color);
        else s_CacheImages[number - 1][anim][frame]->setColor(Color(255, 255, 255, 255));
        s_GameClass->s_RenderTexture->draw(*s_CacheImages[number - 1][anim][frame]);
    }
}

void Monsters::createMaskTransparent(int r, int g, int b)
{
    map<int, map<string, map<int, Texture*> > >::iterator iter_, iter2_;
    for (iter_ = s_Bitmaps.begin(), iter2_ = s_Bitmaps.end(); iter_ != iter2_;)
    {
        map<string, map<int, Texture*> >::iterator _iter_, _iter2_;
        for (_iter_ = iter_->second.begin(), _iter2_ = iter_->second.end(); _iter_ != _iter2_;)
        {
            map<int, Texture*>::iterator _iter__, _iter2__;
            for (_iter__ = _iter_->second.begin(), _iter2__ = _iter_->second.end(); _iter__ != _iter2__;)
            {
                if(_iter__->second != 0)
                {
                    //_iter_->second->create_mask(r, g, b);
                    sf::Image tmp_img = _iter__->second->copyToImage();
                    tmp_img.createMaskFromColor(Color(r, g, b));
                    _iter__->second->loadFromImage(tmp_img);
                }
                ++_iter__;
            }
            ++_iter_;
        }
        ++iter_;
    }
}
