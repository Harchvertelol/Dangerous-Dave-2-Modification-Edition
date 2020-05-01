#ifndef WORKFUNCTION_H
#define WORKFUNCTION_H
#include <iostream>
#include <sstream>
#include <map>
#include "Square.h"

using namespace std;

namespace WorkFunction
{
    namespace WordFunction
    {
        string FirstWord(string);
        string RemoveFirstWord(string);
    };
    namespace ParserFunction
    {
        string getNameMainVariable(string);
        string getNameSecondaryVariable(string);
        string getValueSecondaryVariable(string);
        int splitMass(map<int,int>*,int,int,string,string);
        int splitMassString(map<int,string>*,int,int,string,string);
        string addMainVariableString(string,string,string);
        string addSecondaryVariableString(string,string,string,string);
    };
    namespace ConvertFunction
    {
        string itos(int);
    };
    namespace MathFunction
    {
        int roundNumber(int,int,int);
    };
    namespace GameFunction
    {
        bool testCollision(int,int,int,int,Square,Square,bool stand = false, string where = "up");
    };
};
#endif
