#include "WorkFunctions.h"

#include <regex>

#include <cstdlib>
#include <fstream>

#ifdef _LINUX_
#include <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
#include <io.h>
#include <direct.h>
#endif

using namespace std;

string WorkFunctions::WordFunctions::FirstWord(string str)
{
    if(str == "" || str.find(" ") == string::npos) return str;
    return str.substr(0, str.find(" "));
}

string WorkFunctions::WordFunctions::RemoveFirstWord(string str)
{
    if(str == "" || str.find(" ") == string::npos) return "";
    return str.substr(str.find(" ") + 1);
}

string WorkFunctions::WordFunctions::removeSlashes(string str)
{
    str = std::regex_replace(str, std::regex("\\\\n"), "\n");
    str = std::regex_replace(str, std::regex("\\\\\\n"), "\\n");
    return str;
}

string WorkFunctions::ConvertFunctions::itos(int s_Number)
{
    stringstream s_Str;
    s_Str << s_Number;
    return s_Str.str();
}

string WorkFunctions::ConvertFunctions::ftos(float s_Number)
{
    stringstream s_Str;
    s_Str << s_Number;
    return s_Str.str();
}

string WorkFunctions::ParserFunctions::getNameMainVariable(string str)
{
    if( str == "" || ( str.find("[") == string::npos || str.find("[") != 0 )
       || (str.find("]") == string::npos || str.find("]") != str.size() - 1) ) return "";
    return str.substr(1, str.find("]") - 1);
}

string WorkFunctions::ParserFunctions::getNameSecondaryVariable(string str)
{
    if(str == "" || str.find("=") == string::npos || str.find("=") == 0) return "";
    return str.substr(0, str.find("="));
}

string WorkFunctions::ParserFunctions::getValueSecondaryVariable(string str)
{
    if(str == "" || str.find("=") == string::npos) return "";
    return str.substr(str.find("=") + 1);
}

int WorkFunctions::ParserFunctions::splitMass(map<int,int>* mas, int size, int point, string str, string splitter)
{
    while(str.find(splitter) != string::npos)
    {
        (*mas)[point] = atoi( str.substr(0, str.find(splitter)).c_str() );
        str = str.substr(str.find(splitter) + splitter.size());
        point++;
    }
    (*mas)[point] = atoi( str.substr(0, str.find(splitter)).c_str() );
    point++;
    return point;
}

int WorkFunctions::ParserFunctions::splitMassString(map<int, string>* mas, int size, int point, string str, string splitter)
{
    while(str.find(splitter) != string::npos)
    {
        (*mas)[point] = str.substr(0, str.find(splitter));
        str = str.substr(str.find(splitter) + splitter.size());
        point++;
    }
    (*mas)[point] = str.substr(0, str.find(splitter));
    point++;
    return point;
}

string WorkFunctions::ParserFunctions::addMainVariableString(string str, string name, string splitter)
{
    return str + "[" + name + "]" + splitter;
}

string WorkFunctions::ParserFunctions::addSecondaryVariableString(string str, string name, string value, string splitter)
{
    return str + name + "=" + value + splitter;
}

string WorkFunctions::ParserFunctions::cryptString(string str, string key)
{
    string ret_str = str;
    string tmp_key = key;
    char shift_key = tmp_key[0];
    for(int i = 0; i < ret_str.size(); i++)
    {
        ret_str[i] += tmp_key[i % tmp_key.size()];
        for(int j = 0; j < tmp_key.size(); j++) tmp_key[j] += shift_key;
    }
    return ret_str;
}

string WorkFunctions::ParserFunctions::decryptString(string str, string key)
{
    string ret_str = str;
    string tmp_key = key;
    char shift_key = tmp_key[0];
    for(int i = 0; i < ret_str.size(); i++)
    {
        ret_str[i] -= tmp_key[i % tmp_key.size()];
        for(int j = 0; j < tmp_key.size(); j++) tmp_key[j] += shift_key;
    }
    return ret_str;
}

int WorkFunctions::MathFunctions::roundNumber(int number, int divide, int change)
{
    for(; number % divide != 0; number += change);
    return number;
}

bool WorkFunctions::FileFunctions::isFileExists(string name)
{
    return access(name.c_str(), 0) != -1;
}

bool WorkFunctions::FileFunctions::createFolders(string folder, string foldersplitter)
{
    string tempfolder = "";
    map<int,string> mas;
    int sizemas = WorkFunctions::ParserFunctions::splitMassString(&mas, 0, 0, folder, foldersplitter);
    for(int i = 0; i < sizemas; i++)
    {
        tempfolder += mas[i];
        #ifdef _LINUX_
        mkdir(tempfolder.c_str(), S_IRWXU);
        #else
        mkdir(tempfolder.c_str());
        #endif // _LINUX_
        tempfolder += foldersplitter;
    }
    return true;
}

string WorkFunctions::FileFunctions::fileToString(string file)
{
    ifstream file_read(file.c_str(), ios::binary);
    if(!file_read)
    {
        cerr<< "Error open file " << file << "!" << endl;
        file_read.close();
        return "";
    }
    stringstream ss;
    ss << file_read.rdbuf();
    return ss.str();
}

bool WorkFunctions::GameFunctions::testCollision(int x, int y, int x1, int y1, Square ent, Square ent1, bool stand, string where)
{
    int newLX = x + ent.s_XL;
    int newLY = y + ent.s_YL;
    int newRX = x + ent.s_XR;
    int newRY = y + ent.s_YR;
    int newLX1 = x1 + ent1.s_XL;
    int newLY1 = y1 + ent1.s_YL;
    int newRX1 = x1 + ent1.s_XR;
    int newRY1 = y1 + ent1.s_YR;
    bool test1 = false, test2 = false;
    if( ( (newLX <= newLX1) && (newRX >= newLX1) ) || ( (newLX <= newRX1) && (newRX >= newRX1) ) || ( (newLX >= newLX1) && (newRX <= newRX1) ) ) test1 = true;
    if( ( (newLY <= newLY1) && (newRY >= newLY1) ) || ( (newLY <= newRY1) && (newRY >= newRY1) ) || ( (newLY >= newLY1) && (newRY <= newRY1) ) ) test2 = true;
    if(stand == false) return (test1 && test2);
    if(where == "up") return ( test1 && (newRY + 1 == newLY1) );
    else if(where == "right") return ( test2 && (newRX + 1 == newLX1) );
    else if(where == "down") return ( test1 && (newRY1 + 1 == newLY) );
    else if(where == "left") return ( test2 && (newRX1 + 1 == newLX) );
    return false;
}
