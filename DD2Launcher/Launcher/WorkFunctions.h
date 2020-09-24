#ifndef WORKFUNCTION_H
#define WORKFUNCTION_H
#include <iostream>
#include <sstream>
#include <map>

#include "Defines.h"

#include "Square.h"

using namespace std;

namespace WorkFunctions
{
    namespace WordFunctions
    {
        string FirstWord(string);
        string RemoveFirstWord(string);
    };
    namespace ParserFunctions
    {
        string getNameMainVariable(string);
        string getNameSecondaryVariable(string);
        string getValueSecondaryVariable(string);
        int splitMass(map<int,int>*,int,int,string,string);
        int splitMassString(map<int,string>*,int,int,string,string);
        string addMainVariableString(string,string,string);
        string addSecondaryVariableString(string,string,string,string);
    };
    namespace ConvertFunctions
    {
        string itos(int);
        std::string ftos(float number);
    };
    namespace MathFunctions
    {
        int roundNumber(int,int,int);
    };
    //! Функции работы с файлами.
    namespace FileFunctions
    {
        bool isFileExists(std::string name);
        bool createFolders(std::string folder, std::string foldersplitter = STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER);
    };
    namespace GameFunctions
    {
        bool testCollision(int,int,int,int,Square,Square,bool stand = false, string where = "up");
    };
};
#endif
