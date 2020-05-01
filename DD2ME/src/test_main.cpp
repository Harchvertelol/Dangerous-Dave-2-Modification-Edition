#include "canvas.cpp"
using namespace cnv;
#include <iostream>
using namespace std;

Window w(20*16, 12.5*16, cnv::nostart);
int state = 1;

static LRESULT CALLBACK s1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_LEFT: //стрелка влево
                        state = 2;
                        w.clear();
                        w.draw(Image("test.bmp", 0, 0, 64, 64, 16, 16)); //рисуем изображение в координатах 100,100
                        w.paint();
                        break;
                }
                break;
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}

static LRESULT CALLBACK s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_KEYDOWN: //сообщение WM_KEYDOWN - пользователь нажал какую-то клавишу
                switch(wParam) //в параметре wParam код клавишы. Рассматриваем код
                {
                    case VK_LEFT: //стрелка влево
                        state = 1;
                        w.clear();
                        w.draw(Image("test.bmp", 32, 32, 0, 64, 16, 16)); //рисуем изображение в координатах 100,100
                        w.paint();
                        break;
                }
                break;
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //все остальные сообщения идут сюда. Лучше не трогать =)
}

static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //рассматриваем ИМЯ сообщения
    {
        case WM_PAINT: //сообщение WM_PAINT - требуется перерисовать окно
            w.paint(); //просим Canvas перерисоваться
            break;

        case WM_DESTROY: //WM_DESTROY - пользователь нажал на крестик на окне (закрыть)
            PostQuitMessage(0); //говорим операционной системе, что она может закрыть нашу программу
            break;
    }
    if(state == 1) return s1(hwnd, uMsg, wParam, lParam);
    else if(state == 2) return s2(hwnd, uMsg, wParam, lParam);
    return NULL;
}

/*int main()
{
        w.start(MyWndProc); //запускаем Окно и передаем функцию-обработчик сообщений
        w.enable_frame_buffer(); //активизируем кадровый буффер

        //начинаем СЛУШАТЬ сообщения от операционной системы
        MSG msg; //переменная для сообщения
        while (GetMessage(&msg, NULL, 0, 0)) //пока удается считать сообщение
        {
            TranslateMessage(&msg); //обработка сообщения
            DispatchMessage(&msg); //вот тут вызовется MyWndProc и обработает msg
        }

        return 0;
}*/

int main()
{
        Window w(500, 500); //Размер - 500 на 500 пикселей
        Image* tiles = new Image("test.bmp",0,0);
        tiles->toX = 32;
        tiles->toY= 32;
        tiles->src_x = 32;
        tiles->src_y = 64;
        tiles->dst_h = 16;
        tiles->dst_w = 16;
        w.draw(*tiles);
        system("pause");
        tiles->toX = 16;
        tiles->toY= 16;
        tiles->src_x = 64;
        tiles->src_y = 64;
        tiles->dst_h = 16;
        tiles->dst_w = 16;
        w.draw(*tiles);
        system("pause");
        return 0;
}
