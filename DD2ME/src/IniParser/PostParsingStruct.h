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
            //! Получение всех переменных из блока.
            /*! \param A Значение блока.
            \return Возвращает map, содержащий все переменные из блока.
            */
            std::map<std::string, std::string>& getMapVariables(std::string A);
            //! Добавляет все значения из указанного контейнера в текущий.
            /*! \param pps Откуда добавить все значения.
            */
            void addPostParsingStruct(PostParsingStruct* pps);
            //! Проверяет существование блока.
            /*! \param A Название блока.
            */
            bool isExists(std::string A);
            //! Проверяет существование переменной.
            /*! \param A Название блока.
            \param B Название переменной.
            */
            bool isExists(std::string A, std::string B);
            //! Удаляет переменную.
            /*! \param A Название блока.
            \param B Название переменной.
            */
            void remove(std::string A, std::string B);
            //! Удаляет блок.
            /*! \param A Название блока.
            */
            void remove(std::string A);
            //! Очищает все переменные.
            void clear();
        private:
            std::map<std::string, std::map<std::string, std::string> > s_Variables;
    };
};
#endif
