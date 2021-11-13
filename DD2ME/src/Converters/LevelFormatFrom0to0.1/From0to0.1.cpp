#include <iostream>
#include <cstdlib>

#include "IniParser/ParserInfoFile.h"
#include "Level.h"
#include "WorkFunctions.h"

using namespace IniParser;
using namespace WorkFunctions;
using namespace ConvertFunctions;

int main()
{
    ParserInfoFile prs;
    PostParsingStruct* levelsInf = prs.getParsedFromFile("levels.dat");
    int nol = atoi( levelsInf->getValue("info", "numberoflevels").c_str() );
    for(int i = 1; i <= nol; i++)
    {
        Level q;
        q.convertLevel(itos(i));
    }
    levelsInf->setValue("info", "levelsformat", "0.1");
    //system("copy levels.dat result/levels.dat");
    prs.writeParsedToFile(levelsInf, "result/levels.dat");
    system("pause");
    return 0;
}
