#define CANVAS_NOSTART
#include <ctime>

#include "canvas.hpp"
using namespace cnv;

#include "MainServer.h"

#include "IniParser/ParserInfoFile.h"
#include "IniParser/PostParsingStruct.h"

#include "WorkFunctions.h"

int main(int argc, char** argv)
{
    srand((unsigned)time(NULL));
    MainServer* ms = new MainServer;
    try
    {
        if(!ms->load())
        {
            cout<<"Error loading!"<<endl;
            system("pause");
            return 0;
        }
        if(!ms->run())
        {
            cout<<"Error running!"<<endl;
            system("pause");
            return 0;
        }
        delete ms;
    }
    catch(std::exception& e)
    {
        cout<<"Exception raised: "<<e.what()<<endl;
    }
    system("pause");
    return 0;
}
