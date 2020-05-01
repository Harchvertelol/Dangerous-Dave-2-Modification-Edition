#include "Gui.h"

#include "Game.h"

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
    stringstream s;
    int k = 0;
    int shift = 15;
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDraw") == "true")
    {
        s.str("");
        s<<"FPS game draw: "<<s_GameClass->s_GameDrawFPS<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDrawMaximal") == "true")
    {
        s.str("");
        s<<"FPS game draw maximal: "<<s_GameClass->s_GameDrawFPSMaximal<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSGameDrawMinimal") == "true")
    {
        s.str("");
        s<<"FPS game draw minimal: "<<s_GameClass->s_GameDrawFPSMinimal<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnical") == "true")
    {
        s.str("");
        s<<"FPS technical: "<<s_GameClass->s_TechnicalFPS<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnicalMaximal") == "true")
    {
        s.str("");
        s<<"FPS technical maximal: "<<s_GameClass->s_TechnicalFPSMaximal<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPSTechnicalMinimal") == "true")
    {
        s.str("");
        s<<"FPS technical minimal: "<<s_GameClass->s_TechnicalFPSMinimal<<endl;
        s_GameClass->s_Window->draw(Label(s.str(), 0, k, 10), Brush(0, 0, 0));
        k += shift;
    }
}

void Gui::drawGuiState2()
{
    //...
}

void Gui::drawGuiState3()
{
    int shiftX = 0;
    map<int, Creature*>::iterator iter;
    for ( iter = s_GameClass->s_GameInfo->s_Daves.begin(); iter != s_GameClass->s_GameInfo->s_Daves.end(); iter++)
    {
        if(iter->second->s_NickName != "")
        {
            shiftX = 8-(iter->second->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() )/2;
            s_GameClass->s_Window->draw(CnvRectangle(shiftX + iter->second->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
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
                                                    atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentB").c_str() )));
        }
    }
    //...
    if(s_GameClass->s_GameInfo->s_MyDave->s_NickName != "")
    {
        shiftX = 8-(s_GameClass->s_GameInfo->s_MyDave->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() )/2;
        s_GameClass->s_Window->draw(CnvRectangle(shiftX + s_GameClass->s_GameInfo->s_MyDave->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
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
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentB").c_str() )));
    }
    s_GameClass->s_Data->s_Dave->drawBandolier(s_GameClass->s_GameInfo->s_MyDave->s_Cartridges, 8, 8);
    drawFPS();
}
