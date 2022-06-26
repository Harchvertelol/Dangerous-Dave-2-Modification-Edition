#include <fstream>

#include "IniParser/ParserInfoFile.h"
#include "WorkFunctions.h"

using namespace std;
using namespace IniParser;
using namespace WorkFunctions;
using namespace ConvertFunctions;
using namespace FileFunctions;
using namespace ParserFunctions;

void cryptFile(string file, string key)
{
    ParserInfoFile prs;
    prs.setCryptKey(key);
    prs.setCryptedStatus(true);
    PostParsingStruct* pps = prs.getParsedFromFile(file);
    WorkFunctions::FileFunctions::createFolders("result", ";");
    prs.writeParsedToFile(pps, "result/" + file);
    delete pps;
    cout << "File crypted: " << file << endl;
}

int main()
{
    string key = "12dfslgmjiowekjmfpowf;lksd;fl3";
    ParserInfoFile prs;
    prs.setCryptKey(key);
    prs.setCryptedStatus(true);
    PostParsingStruct* levelsInf = prs.getParsedFromFile("levels.dat");
    int nol = atoi( levelsInf->getValue("info", "numberoflevels").c_str() );
    for(int i = 1; i <= nol; i++) cryptFile(itos(i) + ".lev", key);
    //prs.writeParsedToFile(levelsInf, "result/levels.dat");
    return 0;
}
