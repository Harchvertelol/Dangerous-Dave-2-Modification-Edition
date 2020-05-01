#include "canvas.cpp"
using namespace cnv;
#include <iostream>
using namespace std;

Window w(20*16, 12.5*16, cnv::nostart);
int state = 1;

static LRESULT CALLBACK s1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //������������� ��� ���������
    {
        case WM_KEYDOWN: //��������� WM_KEYDOWN - ������������ ����� �����-�� �������
                switch(wParam) //� ��������� wParam ��� �������. ������������� ���
                {
                    case VK_LEFT: //������� �����
                        state = 2;
                        w.clear();
                        w.draw(Image("test.bmp", 0, 0, 64, 64, 16, 16)); //������ ����������� � ����������� 100,100
                        w.paint();
                        break;
                }
                break;
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //��� ��������� ��������� ���� ����. ����� �� ������� =)
}

static LRESULT CALLBACK s2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //������������� ��� ���������
    {
        case WM_KEYDOWN: //��������� WM_KEYDOWN - ������������ ����� �����-�� �������
                switch(wParam) //� ��������� wParam ��� �������. ������������� ���
                {
                    case VK_LEFT: //������� �����
                        state = 1;
                        w.clear();
                        w.draw(Image("test.bmp", 32, 32, 0, 64, 16, 16)); //������ ����������� � ����������� 100,100
                        w.paint();
                        break;
                }
                break;
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //��� ��������� ��������� ���� ����. ����� �� ������� =)
}

static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) //������������� ��� ���������
    {
        case WM_PAINT: //��������� WM_PAINT - ��������� ������������ ����
            w.paint(); //������ Canvas ��������������
            break;

        case WM_DESTROY: //WM_DESTROY - ������������ ����� �� ������� �� ���� (�������)
            PostQuitMessage(0); //������� ������������ �������, ��� ��� ����� ������� ���� ���������
            break;
    }
    if(state == 1) return s1(hwnd, uMsg, wParam, lParam);
    else if(state == 2) return s2(hwnd, uMsg, wParam, lParam);
    return NULL;
}

/*int main()
{
        w.start(MyWndProc); //��������� ���� � �������� �������-���������� ���������
        w.enable_frame_buffer(); //������������ �������� ������

        //�������� ������� ��������� �� ������������ �������
        MSG msg; //���������� ��� ���������
        while (GetMessage(&msg, NULL, 0, 0)) //���� ������� ������� ���������
        {
            TranslateMessage(&msg); //��������� ���������
            DispatchMessage(&msg); //��� ��� ��������� MyWndProc � ���������� msg
        }

        return 0;
}*/

int main()
{
        Window w(500, 500); //������ - 500 �� 500 ��������
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
