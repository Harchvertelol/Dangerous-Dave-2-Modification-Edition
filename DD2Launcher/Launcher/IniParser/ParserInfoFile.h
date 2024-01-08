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
            \param prs Указатель на PostParsingStruct, куда произвести запись. Если = 0, то создаётся новый.
            \param log Вывод лога ошибок в консоль.
            \return Возвращает указатель на PostParsingStruct. Он равен NULL, если обработка прошла неудачно.
            */
            PostParsingStruct* getParsedFromFile(std::string filename, PostParsingStruct* prs = 0, bool log = true);
            //! Добавление результата обработки из файла.
            /*! \param prs Указатель на PostParsingStruct, куда мы добавляем переменные из файла.
            \param filename Имя файла, который надо обработать.
            \param log Вывод лога ошибок в консоль.
            \return Возвращает указатель на PostParsingStruct. Он равен NULL, если обработка прошла неудачно.
            */
            void addParsedFromFile(PostParsingStruct* prs, std::string filename, bool log = true);
            //! Добавление результата обработки из строки.
            /*! \param str_s Строка, которую надо обработать.
            \param splitter Разделитель в строке вместо переноса на новую строку.
            \param prs Указатель на PostParsingStruct, куда мы добавляем переменные из строки.
            \return Возвращает указатель на PostParsingStruct. Он равен NULL, если обработка прошла неудачно.
            */
            void addParsedFromString(std::string str_s, std::string splitter, PostParsingStruct* prs);
            //! Добавление результата обработки из PostParsingStruct.
            /*! \param pps_main Указатель на PostParsingStruct, куда добавляются данные.
            \param pps_add Указатель на PostParsingStruct, откуда берутся данные.
            */
            void addParsedFromPostParsingStruct(PostParsingStruct* pps_main, PostParsingStruct* pps_add);
            //! Получение результата обработки из строки.
            /*! \param str Строка, которую надо обработать.
            \param splitter Разделитель в строке вместо переноса на новую строку.
            */
            PostParsingStruct* getParsedFromString(std::string str, std::string splitter, PostParsingStruct* prs = 0, bool fixbrbn = false);
            //! Конвертация PostParsingStruct в строку.
            /*! \param pps Конвертируемый PostParsingStruct.
            \param splitter Разделитель в строке вместо переноса на новую строку.
            \param prs Указатель на PostParsingStruct, куда произвести запись. Если = 0, то создаётся новый.
            \param fixbrbn Заменять ли в строке последовательность \r\n на \n после дешифровки (работает только если включена шифровка).
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
            //! Включить или выключить шифрование.
            /*! \param crypted Статус устанавливаемого шифрования.
            \return Возвращает true в случае успеха, в противном случае возвращает false (например, если не установлен ключ шифрования).
            */
            bool setCryptedStatus(bool crypted);
            //! Получить статус шифрования.
            /*! \return Возвращает true в случае включённого шифрования, в противном случае возвращает false.
            */
            bool isCrypted();
            //! Установить ключ шифрования.
            /*! \param crypt_key Ключ шифрования. Если равен пустой строке, шифрование автоматически выключается.
            */
            void setCryptKey(std::string crypt_key);
            //! Получить ключ шифрования.
            std::string getCryptKey();
        private:
            std::string s_CryptKey;
            bool s_IsCrypted;
    };
};
#endif
