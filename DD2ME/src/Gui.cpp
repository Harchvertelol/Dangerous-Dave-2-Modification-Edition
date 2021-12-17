#include "Gui.h"

#include <SFML/Graphics.hpp>

#include "Game.h"

#include "WorkFunctions.h"

using namespace std;

using namespace sf;

Gui::Gui(Game* gameclass):
    s_GameClass(gameclass)
{
    //...
}

Gui::~Gui()
{
    //...
}

void Gui::drawFPS()
{
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPS") != "true" || s_GameClass->s_IniFile->getValue("FPS", "ShowFPSMode") != "graphic") return;
    Font main_fnt = s_GameClass->s_Data->s_GuiData->getFont("main");
    stringstream s;
    int k = 0;
    int shift = 15;
    int fps_chr_size = 10;
    Color fps_chr_color = Color(0, 0, 0, 255);
    Text txt;
    txt.setFont(main_fnt);
    txt.setFillColor(fps_chr_color);
    txt.setCharacterSize(fps_chr_size);
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDraw") == "true")
    {
        s.str("");
        s<<"FPS game draw: "<<s_GameClass->s_GameDrawFPS<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDrawMaximal") == "true")
    {
        s.str("");
        s<<"FPS game draw maximal: "<<s_GameClass->s_GameDrawFPSMaximal<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDrawMinimal") == "true")
    {
        s.str("");
        s<<"FPS game draw minimal: "<<s_GameClass->s_GameDrawFPSMinimal<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnical") == "true")
    {
        s.str("");
        s<<"FPS technical: "<<s_GameClass->s_TechnicalFPS<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnicalMaximal") == "true")
    {
        s.str("");
        s<<"FPS technical maximal: "<<s_GameClass->s_TechnicalFPSMaximal<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnicalMinimal") == "true")
    {
        s.str("");
        s<<"FPS technical minimal: "<<s_GameClass->s_TechnicalFPSMinimal<<endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        //s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
}

void Gui::drawGuiState2(int screennumber)
{
    string screenname = "screen_" + WorkFunctions::ConvertFunctions::itos(screennumber + 1);
    int levelnamecoordX = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "levelnamecoordX").c_str());
    int levelnamecoordY = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "levelnamecoordY").c_str());
    int levelnamesize = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "levelnamesize").c_str());
    int secretscoordX = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "secretscoordX").c_str());
    int secretscoordY = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "secretscoordY").c_str());
    int secretssize = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "secretssize").c_str());
    int scorecoordX = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "scorecoordX").c_str());
    int scorecoordY = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "scorecoordY").c_str());
    int scoresize = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "scoresize").c_str());
    int highscorecoordX = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "highscorecoordX").c_str());
    int highscorecoordY = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "highscorecoordY").c_str());
    int highscoresize = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "highscoresize").c_str());
    int livescoordX = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livescoordX").c_str());
    int livescoordY = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livescoordY").c_str());
    int livesmaxobject = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livesmaxobject").c_str());
    string livesnamedavestate = s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livesnamedavestate");
    int livesframedavestate = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livesframedavestate").c_str());
    string levelname = "";
    if(s_GameClass->s_Data->s_Level->s_Params->isExists("info", "name")) levelname = s_GameClass->s_Data->s_Level->s_Params->getValue("info", "name");
    /*if(levelname == "") s_GameClass->s_Window->draw(Label("Level " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel), levelnamecoordX, levelnamecoordY, levelnamesize), Pen(0, 0, 0));
    else s_GameClass->s_Window->draw(Label(levelname, levelnamecoordX, levelnamecoordY, levelnamesize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label("Secrets: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_Data->s_Level->s_Params->getMapVariables()["Secrets"].size()), secretscoordX, secretscoordY, secretssize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label(WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_MyDave->s_CurrentPoints), scorecoordX, scorecoordY, scoresize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label(WorkFunctions::ConvertFunctions::itos(0), highscorecoordX, highscorecoordY, highscoresize), Pen(0, 0, 0));*/
    Font main_fnt = s_GameClass->s_Data->s_GuiData->getFont("main");
    Text txt;
    txt.setFont(main_fnt);

    txt.setFillColor(Color(0, 0, 0, 255));
    txt.setCharacterSize(levelnamesize);
    txt.setPosition(Vector2f(levelnamecoordX, levelnamecoordY));
    string level_str = "Level " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel);
    if(levelname != "") level_str = levelname;
    txt.setString(level_str);
    s_GameClass->s_RenderTexture->draw(txt);

    string secrets_text = "Secrets: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_Data->s_Level->s_Params->getMapVariables()["Secrets"].size());
    txt.setFillColor(Color(0, 0, 0, 255));
    txt.setCharacterSize(secretssize);
    txt.setPosition(Vector2f(secretscoordX, secretscoordY));
    txt.setString(secrets_text);
    s_GameClass->s_RenderTexture->draw(txt);

    string score_text = WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_MyDave->s_CurrentPoints);
    txt.setFillColor(Color(0, 0, 0, 255));
    txt.setCharacterSize(scoresize);
    txt.setPosition(Vector2f(scorecoordX, scorecoordY));
    txt.setString(score_text);
    s_GameClass->s_RenderTexture->draw(txt);


    string highscore_text = WorkFunctions::ConvertFunctions::itos(0);
    txt.setFillColor(Color(0, 0, 0, 255));
    txt.setCharacterSize(highscoresize);
    txt.setPosition(Vector2f(highscorecoordX, highscorecoordY));
    txt.setString(highscore_text);
    s_GameClass->s_RenderTexture->draw(txt);

    for(int i = 0; i < s_GameClass->s_GameInfo->s_CurrentLives - 1 && i < livesmaxobject; i++)
    {
        s_GameClass->s_Data->s_Dave->drawDave(livesnamedavestate, livesframedavestate, livescoordX + i * 16, livescoordY);
    }
}

void Gui::drawGuiState3()
{
    int shiftX = 0;
    map<int, CreatureDave*>::iterator iter;
    for ( iter = s_GameClass->s_GameInfo->s_Daves.begin(); iter != s_GameClass->s_GameInfo->s_Daves.end(); iter++)
    {
        if(iter->second->s_NickName != "")
        {
            shiftX = 8-(iter->second->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() )/2;
            /*s_GameClass->s_Window->draw(CnvRectangle(shiftX + iter->second->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                                 iter->second->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                 shiftX + iter->second->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ) +
                                                 iter->second->s_NickName.size()*2 + (iter->second->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                 iter->second->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ) +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeY").c_str() )),
                                                 Pen(PS_SOLID,
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinesize").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorR").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorG").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorB").c_str() )),
                                                 Brush(BS_SOLID,
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorR").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorG").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorB").c_str() )));
            s_GameClass->s_Window->draw(Label(iter->second->s_NickName,
                                                 shiftX + iter->second->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                                 iter->second->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeY").c_str() ),
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                    atof( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "nameorient").c_str() )),
                                                 Pen(PS_SOLID, atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "sizelineletter").c_str() ),
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentR").c_str() ),
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentG").c_str() ),
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentB").c_str() )));*/
        }
    }
    //...
    if(s_GameClass->s_GameInfo->s_MyDave->s_NickName != "")
    {
        shiftX = 8-(s_GameClass->s_GameInfo->s_MyDave->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() )/2;
        /*s_GameClass->s_Window->draw(CnvRectangle(shiftX + s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                                 s_GameClass->s_GameInfo->s_MyDave->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                 shiftX + s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ) +
                                                 s_GameClass->s_GameInfo->s_MyDave->s_NickName.size()*2 + (s_GameClass->s_GameInfo->s_MyDave->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                 s_GameClass->s_GameInfo->s_MyDave->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ) +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeY").c_str() )),
                                                 Pen(PS_SOLID,
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinesize").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorR").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorG").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundlinecolorB").c_str() )),
                                                 Brush(BS_SOLID,
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorR").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorG").c_str() ),
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "backgroundcolorB").c_str() )));
        s_GameClass->s_Window->draw(Label(s_GameClass->s_GameInfo->s_MyDave->s_NickName,
                                             shiftX + s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                             atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                             s_GameClass->s_GameInfo->s_MyDave->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeY").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                atof( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "nameorient").c_str() )),
                                             Pen(PS_SOLID, atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "sizelineletter").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentR").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentG").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentB").c_str() )));*/
    }
    s_GameClass->s_Data->s_Dave->drawBandolier(s_GameClass->s_GameInfo->s_MyDave->s_Cartridges, 8, 8);
    drawFPS();
}
