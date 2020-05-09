#ifndef POSTPARSINGSTRUCT_H
#define POSTPARSINGSTRUCT_H

#include <map>

#include <string>

namespace IniParser
{
    //! Класс, который заполняется в результате обработки ini-файла.
    /*! Данный класс содержит информацию в результате обработки файла с помощью ParserInfoFile.
    */
    class PostParsingStruct
    {
        public:
            //! Стандартный конструктор.
            PostParsingStruct();
            //! Стандартный деструктор.
            ~PostParsingStruct();
            //! Получение значения переменной.
            /*! \param A Значение блока, откуда берём переменную.
            \param B Название переменной.
            \return Возвращает значение переменной.
            */
            std::string getValue(std::string A, std::string B);
            //! Установка значения переменной.
            /*! \param A Значение блока, где берём переменную.
            \param B Название переменной.
            \param C Значение, которое устанавливаем переменной.
            */
            void setValue(std::string A, std::string B, std::string C);
            //! Получение всех переменных.
            /*! \return Возвращает map, содержащий все переменные.
            */
            std::map<std::string, std::map<std::string, std::string> >& getMapVariables();
            //! Добавляет все значения из указанного контейнера в текущий.
            /*! \param pps Откуда добавить все значения.
            */
            void addPostParsingStruct(PostParsingStruct* pps);
        private:
            std::map<std::string, std::map<std::string, std::string> > s_Variables;
    };
};
#endif
