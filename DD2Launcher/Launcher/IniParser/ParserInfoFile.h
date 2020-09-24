#ifndef PARSERINFOFILE_H
#define PARSERINFOFILE_H

#include <string>

#include "PostParsingStruct.h"

namespace IniParser
{
    //! Класс для обработки ini-файлов.
    /*! Данный класс загружает и обрабатывает ini-файлы, выдавая результатом заполненную PostParsingStruct.
    */
    class ParserInfoFile
    {
        public:
            //! Стандартный конструктор.
            ParserInfoFile();
            //! Стандартный деструктор.
            ~ParserInfoFile();
            //! Получение результата обработки из файла.
            /*! \param filename Имя файла, который надо обработать.
            \param log Вывод лога ошибок в консоль.
            \return Возвращает указатель на PostParsingStruct. Он равен NULL, если обработка прошла неудачно.
            */
            PostParsingStruct* getParsedFromFile(std::string filename, bool log = true);
            //! Получение результата обработки из строки.
            /*! \param str Строка, которую надо обработать.
            \param splitter Разделитель в строке вместо переноса на новую строку.
            \return Возвращает указатель на PostParsingStruct. Он равен NULL, если обработка прошла неудачно.
            */
            PostParsingStruct* getParsedFromString(std::string str, std::string splitter);
            //! Конвертация PostParsingStruct в строку.
            /*! \param pps Конвертируемый PostParsingStruct.
            \param splitter Разделитель в строке вместо переноса на новую строку.
            \return Возвращает строку, которая является результатом конвертации.
            */
            std::string convertPostParsingStructToString(PostParsingStruct* pps, std::string splitter);
            //! Запись PostParsingStruct в файл.
            /*! \param pps Записываемая PostParsingStruct.
            \param filename Имя файла, куда идёт запись.
            \param log Вывод лога ошибок в консоль.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool writeParsedToFile(PostParsingStruct* pps, std::string filename, bool log = true);
    };
};
#endif
