#include "WorkFunction.h"

#include <stdlib.h>

string WorkFunction::WordFunction::FirstWord(string str)
{
    if(str == "" || str.find(" ") == string::npos) return str;
    return str.substr(0, str.find(" "));
}

string WorkFunction::WordFunction::RemoveFirstWord(string str)
{
    if(str == "" || str.find(" ") == string::npos) return "";
    return str.substr(str.find(" ") + 1);
}

string WorkFunction::ConvertFunction::itos(int s_Number)
{
    stringstream s_Str;
    s_Str << s_Number;
    return s_Str.str();
}

string WorkFunction::ParserFunction::getNameMainVariable(string str)
{
    if( str == "" || ( str.find("[") == string::npos || str.find("[") != 0 )
       || (str.find("]") == string::npos || str.find("]") != str.size() - 1) ) return "";
    return str.substr(1, str.find("]") - 1);
}

string WorkFunction::ParserFunction::getNameSecondaryVariable(string str)
{
    if(str == "" || str.find("=") == string::npos || str.find("=") == 0) return "";
    return str.substr(0, str.find("="));
}

string WorkFunction::ParserFunction::getValueSecondaryVariable(string str)
{
    if(str == "" || str.find("=") == string::npos) return "";
    return str.substr(str.find("=") + 1);
}

int WorkFunction::ParserFunction::splitMass(map<int,int>* mas, int size, int point, string str, string splitter)
{
    while(str.find(splitter) != string::npos)
    {
        (*mas)[point] = atoi( str.substr(0, str.find(splitter)).c_str() );
        str = str.substr(str.find(splitter) + 1);
        point++;
    }
    (*mas)[point] = atoi( str.substr(0, str.find(splitter)).c_str() );
    point++;
    return point;
}

int WorkFunction::ParserFunction::splitMassString(map<int,string>* mas, int size, int point, string str, string splitter)
{
    while(str.find(splitter) != string::npos)
    {
        (*mas)[point] = str.substr(0, str.find(splitter)).c_str();
        str = str.substr(str.find(splitter) + 1);
        point++;
    }
    (*mas)[point] = str.substr(0, str.find(splitter)).c_str();
    point++;
    return point;
}

string WorkFunction::ParserFunction::addMainVariableString(string str, string name, string splitter)
{
    return str + "[" + name + "]" + splitter;
}

string WorkFunction::ParserFunction::addSecondaryVariableString(string str, string name, string value, string splitter)
{
    return str + name + "=" + value + splitter;
}

int WorkFunction::MathFunction::roundNumber(int number, int divide, int change)
{
    for(; number % divide != 0; number += change);
    return number;
}

bool WorkFunction::GameFunction::testCollision(int x, int y, int x1, int y1, Square ent, Square ent1, bool stand, string where)
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
