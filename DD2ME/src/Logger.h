#ifndef Logger_H
#define Logger_H

#include <string>
#include "Defines.h"

//! Система ведения логов.
/*! Данная система позволяет обрабатывать различные сообщения, передаваемые ей.
*/
class Logger
{
    public:
        //! Стандартный конструктор.
        Logger();
        //! Стандартный деструктор.
        ~Logger();
        //! Регистрация события.
        /*! \param TypeEvent Тип регистрируемого события.
        \param TextEvent Текст сообщения для логгера.
        \param IsFatal Если имеет значения true, то событие является фатальным. Рекомендуется прекратить работу с программой.
        */
        void registerEvent(LoggerEventType TypeEvent, std::string TextEvent, bool IsFatal = false);
        //! Установка вывода сообщений об ошибках (EVENT_TYPE_ERROR).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_ERROR будут выводиться, иначе - нет.
        */
        void setPrintError(bool value);
        //! Получение состояния вывода сообщений об ошибках (EVENT_TYPE_ERROR).
        /*! \return Возвращает true, если сообщения об ошибках выводятся, иначе false.
        */
        bool getPrintError();
        //! Установка вывода сообщений о логических ошибках (EVENT_TYPE_LOGIC_VIOLATION).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_LOGIC_VIOLATION будут выводиться, иначе - нет.
        */
        void setPrintLogicViolation(bool value);
        //! Получение состояния вывода сообщений о логических ошибках (EVENT_TYPE_LOGIC_VIOLATION).
        /*! \return Возвращает true, если сообщения о логических ошибках выводятся, иначе false.
        */
        bool getPrintLogicViolation();
        //! Установка вывода сообщений, которые являются предупреждениями (EVENT_TYPE_WARNING).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_WARNING будут выводиться, иначе - нет.
        */
        void setPrintWarning(bool value);
        //! Получение состояния вывода сообщений, которые являются предупреждениями (EVENT_TYPE_WARNING).
        /*! \return Возвращает true, если сообщения которые являются предупреждениями, выводятся, иначе false.
        */
        bool getPrintWarning();
        //! Установка вывода стандартных сообщений (EVENT_TYPE_STANDARD).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_STANDARD будут выводиться, иначе - нет.
        */
        void setPrintStandard(bool value);
        //! Получение состояния вывода стандартных сообщений (EVENT_TYPE_STANDARD).
        /*! \return Возвращает true, если стандартные сообщения выводятся, иначе false.
        */
        bool getPrintStandard();
        //! Установка вывода информационных сообщений (EVENT_TYPE_INFO).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_INFO будут выводиться, иначе - нет.
        */
        void setPrintInfo(bool value);
        //! Получение состояния вывода информационных сообщений (EVENT_TYPE_INFO).
        /*! \return Возвращает true, если информационные сообщения выводятся, иначе false.
        */
        bool getPrintInfo();
        //! Установка вывода информационных сообщений (EVENT_TYPE_DEBUG_INFO).
        /*! \param value Если имеет значение true, то сообщения с типом EVENT_TYPE_DEBUG_INFO будут выводиться, иначе - нет.
        */
        void setPrintDebugInfo(bool value);
        //! Получение состояния вывода информационных сообщений (EVENT_TYPE_DEBUG_INFO).
        /*! \return Возвращает true, если информационные сообщения выводятся, иначе false.
        */
        bool getPrintDebugInfo();
        //! Установка вывода событий в консоль.
        /*! \param value Если имеет значение true, то события будут выводиться в консоль, иначе - нет.
        */
        void setConsolePrint(bool value);
        //! Получение состояния вывода событий в консоль.
        /*! \return Возвращает true, если события в консоль выводятся, иначе false.
        */
        bool getConsolePrint();
        //! Установка вывода событий в файл.
        /*! \param value Если имеет значение true, то события будут выводиться в файл, иначе - нет.
        */
        void setFilePrint(bool value);
        //! Получение состояния вывода событий в файл.
        /*! \return Возвращает true, если события в файл выводятся, иначе false.
        */
        bool getFilePrint();
        //! Установка имени лог-файла.
        /*! \param value Имя лог-файла.
        */
        void setLogFileName(std::string value);
        //! Получение имени лог-файла.
        /*! \return Возвращает имя лог-файла.
        */
        std::string getLogFileName();
    private:
        std::string getResultPostProcessStringEvent(LoggerEventType, std::string, bool);
        void registerEventStandardConsole(std::string);
        void registerEventStandardFile(std::string);
        bool s_PrintError;
        bool s_PrintLogicViolation;
        bool s_PrintWarning;
        bool s_PrintStandard;
        bool s_PrintInfo;
        bool s_PrintDebugInfo;
        bool s_PrintToConsole;
        bool s_PrintToFile;
        std::string s_LogFileName;
};
#endif
