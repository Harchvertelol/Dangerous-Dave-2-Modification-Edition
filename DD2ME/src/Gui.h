#ifndef GUI_H
#define GUI_H

class Game;

class Gui
{
    public:
        Gui(Game*);
        ~Gui();
        Game* s_GameClass;
        void drawFPS();
        //...
        void drawGuiState2();
        void drawGuiState3();
};
#endif
