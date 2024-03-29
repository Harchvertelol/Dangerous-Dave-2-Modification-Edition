#include "Gui.h"

#include <SFML/Graphics.hpp>

#include "Game.h"

#include "WorkFunctions.h"

using namespace std;

using namespace sf;

Gui::Gui(Game* gameclass):
    s_GameClass(gameclass),
    s_InfoWindow(0)
{
    s_TGUI = new tgui::Gui;
}

Gui::~Gui()
{
    if(s_TGUI != 0) delete s_TGUI;
}

void Gui::drawFPS()
{
    if(s_GameClass->s_IniFile->getValue("FPS", "ShowFPS") != "true" || s_GameClass->s_IniFile->getValue("FPS", "ShowFPSMode") != "graphic") return;
    sf::Font main_fnt = s_GameClass->s_Data->s_GuiData->getSFMLFont("fps");
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
    if(s_GameClass->s_NetClient->s_NetManager)
    {
        s.str("");
        s << "Ping: "<< s_GameClass->s_NetClient->s_NetManager->getPing() << endl;
        txt.setString(s.str());
        txt.setPosition(Vector2f(0, k));
        s_GameClass->s_RenderTexture->draw(txt);
        k += shift;
    }
}

string Gui::getSecretsText()
{
    string cur_col_secr_str = "";
    if(s_GameClass->s_GameInfo->s_MyPlayer->s_Values->isExists("GS_secrets_visited", "level_" + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel))) cur_col_secr_str = s_GameClass->s_GameInfo->s_MyPlayer->s_Values->getValue("GS_secrets_visited", "level_" + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel));
    int cur_col_secr = 0;
    if(cur_col_secr_str != "")
    {
        map<int, string> map_tmp;
        cur_col_secr = WorkFunctions::ParserFunctions::splitMassString(&map_tmp, 0, 0, cur_col_secr_str, ";") - 1;
    }
    return "Secrets: " + WorkFunctions::ConvertFunctions::itos(cur_col_secr) + "/" + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_Data->s_Level->s_Params->getMapVariables()["Secrets"].size());
}

void Gui::drawGuiState2(int screennumber)
{
    map<int, int> tmp_mas;
    int res_col = 0;
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
    string livesnameplayerstate = s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livesnameplayerstate");
    int livesframeplayerstate = atoi(s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "livesframeplayerstate").c_str());
    string levelname = "";
    if(s_GameClass->s_Data->s_Level->s_Params->isExists("info", "name")) levelname = s_GameClass->s_Data->s_Level->s_Params->getValue("info", "name");
    /*if(levelname == "") s_GameClass->s_Window->draw(Label("Level " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel), levelnamecoordX, levelnamecoordY, levelnamesize), Pen(0, 0, 0));
    else s_GameClass->s_Window->draw(Label(levelname, levelnamecoordX, levelnamecoordY, levelnamesize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label("Secrets: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_Data->s_Level->s_Params->getMapVariables()["Secrets"].size()), secretscoordX, secretscoordY, secretssize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label(WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_MyPlayer->s_CurrentPoints), scorecoordX, scorecoordY, scoresize), Pen(0, 0, 0));
    s_GameClass->s_Window->draw(Label(WorkFunctions::ConvertFunctions::itos(0), highscorecoordX, highscorecoordY, highscoresize), Pen(0, 0, 0));*/
    sf::Font main_fnt = s_GameClass->s_Data->s_GuiData->getSFMLFont("changelevelscreen");
    Text txt;
    txt.setFont(main_fnt);

    txt.setCharacterSize(levelnamesize);
    txt.setPosition(Vector2f(levelnamecoordX, levelnamecoordY));
    string level_str = "Level " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel);
    if(levelname != "") level_str = levelname;
    txt.setString(level_str);
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "textcolorlevelname"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters: textcolorlevelname");
    else txt.setFillColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    s_GameClass->s_RenderTexture->draw(txt);

    string secrets_text = getSecretsText();
    txt.setCharacterSize(secretssize);
    txt.setPosition(Vector2f(secretscoordX, secretscoordY));
    txt.setString(secrets_text);
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "textcolorsecret"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters: textcolorsecret");
    else txt.setFillColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    s_GameClass->s_RenderTexture->draw(txt);

    string score_text = WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_MyPlayer->s_CurrentPoints);
    txt.setFillColor(Color(0, 0, 0, 255));
    txt.setCharacterSize(scoresize);
    txt.setPosition(Vector2f(scorecoordX, scorecoordY));
    txt.setString(score_text);
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "textcolorscore"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters: textcolorscore");
    else txt.setFillColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    s_GameClass->s_RenderTexture->draw(txt);

    string highscore_text = WorkFunctions::ConvertFunctions::itos(0);
    txt.setCharacterSize(highscoresize);
    txt.setPosition(Vector2f(highscorecoordX, highscorecoordY));
    txt.setString(highscore_text);
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_Screens->s_ChangeLevelInfo->getValue(screenname, "textcolorhighscore"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters: textcolorhighscore");
    else txt.setFillColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    s_GameClass->s_RenderTexture->draw(txt);

    for(int i = 0; i < s_GameClass->s_GameInfo->s_CurrentLives - 1 && i < livesmaxobject; i++)
    {
        s_GameClass->s_Data->s_Player->drawPlayer(livesnameplayerstate, livesframeplayerstate, livescoordX + i * 16, livescoordY);
    }
}

void Gui::drawGuiState3()
{
    int shiftX = 0;
    map<int, CreaturePlayer*>::iterator iter;
    for ( iter = s_GameClass->s_GameInfo->s_Players.begin(); iter != s_GameClass->s_GameInfo->s_Players.end(); iter++)
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
    if(s_GameClass->s_GameInfo->s_MyPlayer->s_NickName != "")
    {
        shiftX = 8-(s_GameClass->s_GameInfo->s_MyPlayer->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() )/2;
        /*s_GameClass->s_Window->draw(CnvRectangle(shiftX + s_GameClass->s_GameInfo->s_MyPlayer->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                                 s_GameClass->s_GameInfo->s_MyPlayer->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                 shiftX + s_GameClass->s_GameInfo->s_MyPlayer->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                                 atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ) +
                                                 s_GameClass->s_GameInfo->s_MyPlayer->s_NickName.size()*2 + (s_GameClass->s_GameInfo->s_MyPlayer->s_NickName.size())*atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                 s_GameClass->s_GameInfo->s_MyPlayer->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
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
        s_GameClass->s_Window->draw(Label(s_GameClass->s_GameInfo->s_MyPlayer->s_NickName,
                                             shiftX + s_GameClass->s_GameInfo->s_MyPlayer->s_CoordX - s_GameClass->s_GameInfo->s_ScreenCoordX +
                                             atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordX").c_str() ),
                                             s_GameClass->s_GameInfo->s_MyPlayer->s_CoordY - s_GameClass->s_GameInfo->s_ScreenCoordY +
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namecoordY").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeY").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "namelettersizeX").c_str() ),
                                                atof( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "nameorient").c_str() )),
                                             Pen(PS_SOLID, atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "sizelineletter").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentR").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentG").c_str() ),
                                                atoi( s_GameClass->s_NetClient->s_NetInfo->getValue("gui", "lettercolorcomponentB").c_str() )));*/
    }
    s_GameClass->s_Data->s_Player->drawBandolier(s_GameClass->s_GameInfo->s_MyPlayer->s_Cartridges, 8, 8);
    drawFPS();
}

void Gui::createPopupWindow(string text, int timer)
{
    map<int, int> tmp_mas;
    int res_col = 0;

    tgui::Font main_fnt = s_GameClass->s_Data->s_GuiData->getTGUIFont("popupwindow");

    auto popupWindow = tgui::Panel::create();
    auto label_text = tgui::Label::create();
    text = WorkFunctions::WordFunctions::removeSlashes(text);
    label_text->setText(text);
    label_text->setTextSize(15);
    label_text->setSize("85%", "70%");
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "50%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    label_text->getRenderer()->setFont(main_fnt);
    //label_text->setMaximumTextWidth("85%");
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "textcolorpopupwindow"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for popup window: textcolorpopupwindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    popupWindow->setSize("30%", "10%");
    //popupWindow->setSize(bindSize(label_text));
    popupWindow->setOrigin(0.5, 0.5);
    popupWindow->setPosition("50%", "10%");
    popupWindow->getRenderer()->setBorders(tgui::Borders(1));
    tmp_mas.clear();
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "bordercolorpopupwindow"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for popup window: bordercolorpopupwindow");
    else popupWindow->getRenderer()->setBorderColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    tmp_mas.clear();
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "backgroundcolorpopupwindow"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for popup window: backgroundcolorpopupwindow");
    else popupWindow->getRenderer()->setBackgroundColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    popupWindow->add(label_text);
    popupWindow->showWithEffect(tgui::ShowAnimationType::Fade, atoi(s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "timefadepopupwindow").c_str()));
    //popupWindow->onShowEffectFinish([=](bool isShow){ if(!isShow) popupWindow->getParent()->remove(popupWindow); });
    //s_GameClass->s_Gui->s_TGUI->add(popupWindow);
    //tgui::Timer::scheduleCallback([=](){ popupWindow->hideWithEffect(tgui::ShowAnimationType::Fade, atoi(s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "timefadepopupwindow").c_str())); }, timer);
    popupWindow->onShowEffectFinish([=](bool isShow){ if(!isShow) popupWindow->getParent()->remove(popupWindow); else tgui::Timer::scheduleCallback([=](){ popupWindow->hideWithEffect(tgui::ShowAnimationType::Fade, atoi(s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "timefadepopupwindow").c_str())); }, timer); });
    s_GameClass->s_Gui->s_TGUI->add(popupWindow);
}

void Gui::showInfo()
{
    if(s_InfoWindow != 0)
    {
        s_InfoWindow->getParent()->remove(s_InfoWindow);
        s_InfoWindow = 0;
    }
    tgui::Font main_fnt = s_GameClass->s_Data->s_GuiData->getTGUIFont("infowindow");
    string size_panel_x = "90%", size_panel_y = "90%";
    string size_text_x = "85%", size_text_y = "70%";
    string start_pos_text_x = "50%", start_pos_text_y = "10%";
    int size_text_info_main = 40 * s_GameClass->s_DisplayStruct->s_WindowResolutionX / 1280;
    int size_text_info = 30 * s_GameClass->s_DisplayStruct->s_WindowResolutionX / 1280;
    auto infoWindow = tgui::Panel::create();
    infoWindow->getRenderer()->setBorders(tgui::Borders(1));
    map<int, int> tmp_mas;
    int res_col = 0;
    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "backgroundcolorinfowindow"), ";");
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: backgroundcolorinfowindow");
    else infoWindow->getRenderer()->setBackgroundColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    infoWindow->setSize(size_panel_x.c_str(), size_panel_y.c_str());
    infoWindow->setOrigin(0.5, 0.5);
    infoWindow->setPosition("50%", "50%");

    res_col = WorkFunctions::ParserFunctions::splitMass(&tmp_mas, 0, 0, s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "textcolorinfowindow"), ";");

    auto label_text = tgui::Label::create();
    label_text->setText("Information:");
    label_text->setTextSize(size_text_info_main);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition(start_pos_text_x.c_str(), start_pos_text_y.c_str());
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    label_text = tgui::Label::create();
    string level_text;
    if(s_GameClass->s_Data->s_Level->s_Params->isExists("info", "name")) level_text = s_GameClass->s_Data->s_Level->s_Params->getValue("info", "name");
    if(level_text == "") level_text = WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLevel);
    label_text->setText("Level: " + level_text);
    label_text->setTextSize(size_text_info);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "20%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    label_text = tgui::Label::create();
    label_text->setText("Lives: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_CurrentLives));
    label_text->setTextSize(size_text_info);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "30%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    label_text = tgui::Label::create();
    label_text->setText("Points: " + WorkFunctions::ConvertFunctions::itos(s_GameClass->s_GameInfo->s_MyPlayer->s_CurrentPoints));
    label_text->setTextSize(size_text_info);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "40%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    label_text = tgui::Label::create();
    label_text->setText("Highscore: 0");
    label_text->setTextSize(size_text_info);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "50%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    label_text = tgui::Label::create();
    label_text->setText(getSecretsText());
    label_text->setTextSize(size_text_info);
    label_text->setSize(size_text_x.c_str(), size_text_y.c_str());
    label_text->setOrigin(0.5, 0.5);
    label_text->setPosition("50%", "60%");
    label_text->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label_text->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    if(res_col < 3) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error with gui parameters for info window: textcolorinfowindow");
    else label_text->getRenderer()->setTextColor(tgui::Color(tmp_mas[0], tmp_mas[1], tmp_mas[2], tmp_mas[3]));
    label_text->getRenderer()->setFont(main_fnt);
    infoWindow->add(label_text);

    s_GameClass->s_Gui->s_TGUI->add(infoWindow);
    s_InfoWindow = infoWindow;

    infoWindow->showWithEffect(tgui::ShowAnimationType::Fade, atoi(s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "timefadeinfowindow").c_str()));
}

void Gui::removeInfo(bool immediately)
{
    if(!s_InfoWindow) return;
    if(immediately)
    {
        s_InfoWindow->getParent()->remove(s_InfoWindow);
        s_InfoWindow = 0;
        return;
    }
    s_InfoWindow->hideWithEffect(tgui::ShowAnimationType::Fade, atoi(s_GameClass->s_Data->s_GuiData->s_GuiInfo->getValue("gui", "timefadeinfowindow").c_str()));
    s_InfoWindow->onShowEffectFinish([=](bool isShow){ if(!isShow) s_InfoWindow->getParent()->remove(s_InfoWindow); s_InfoWindow = 0; });
}
