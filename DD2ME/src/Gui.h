#ifndef GUI_H
#define GUI_H
#include <TGUI/TGUI.hpp>

class Game;

class Gui
{
    public:
        Gui(Game*);
        ~Gui();
        Game* s_GameClass;
        tgui::Gui* s_TGUI;
        void drawFPS();
        //...
        void drawGuiState2(int);
        void drawGuiState3();
        void createPopupWindow(std::string, int);
};
#endif
