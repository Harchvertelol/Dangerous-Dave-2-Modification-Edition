#ifndef WORKFUNCTION_H
#define WORKFUNCTION_H
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "Defines.h"

#include "Square.h"

namespace WorkFunctions
{
    namespace WordFunctions
    {
        std::string FirstWord(std::string);
        std::string RemoveFirstWord(std::string);
        std::string removeSlashes(std::string);
    };
    namespace ParserFunctions
    {
        std::string getNameMainVariable(std::string);
        std::string getNameSecondaryVariable(std::string);
        std::string getValueSecondaryVariable(std::string);
        int splitMass(std::map<int, int>*, int, int, std::string, std::string);
        int splitMassString(std::map<int, std::string>*, int, int, std::string, std::string);
        std::string addMainVariableString(std::string, std::string, std::string);
        std::string addSecondaryVariableString(std::string, std::string, std::string, std::string);
        std::string cryptString(std::string, std::string);
        std::string decryptString(std::string, std::string);
    };
    namespace ConvertFunctions
    {
        std::string itos(int);
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
        bool isDirectory(std::string name);
        bool createFolders(std::string folder, std::string foldersplitter = STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER);
        bool createFoldersForFile(std::string fullfilename, std::string foldersplitter = STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER);
        std::string fileToString(std::string);
        std::vector<std::string> getListAllFiles(std::string foldername, std::vector<std::string> push_arr = std::vector<std::string>());
    };
    namespace GameFunctions
    {
        bool testCollision(int,int,int,int,Square,Square,bool stand = false, std::string where = "up");
    };
};
#endif
