#ifndef Logger_H
#define Logger_H

#include <string>
#include "Defines.h"

//! ������� ������� �����.
/*! ������ ������� ��������� ������������ ��������� ���������, ������������ ��.
*/
class Logger
{
    public:
        //! ����������� �����������.
        Logger();
        //! ����������� ����������.
        ~Logger();
        //! ����������� �������.
        /*! \param TypeEvent ��� ��������������� �������.
        \param TextEvent ����� ��������� ��� �������.
        \param IsFatal ���� ����� �������� true, �� ������� �������� ���������. ������������� ���������� ������ � ����������.
        */
        void registerEvent(LoggerEventType TypeEvent, std::string TextEvent, bool IsFatal = false);
        //! ��������� ������ ��������� �� ������� (EVENT_TYPE_ERROR).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_ERROR ����� ����������, ����� - ���.
        */
        void setPrintError(bool value);
        //! ��������� ��������� ������ ��������� �� ������� (EVENT_TYPE_ERROR).
        /*! \return ���������� true, ���� ��������� �� ������� ���������, ����� false.
        */
        bool getPrintError();
        //! ��������� ������ ��������� � ���������� ������� (EVENT_TYPE_LOGIC_VIOLATION).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_LOGIC_VIOLATION ����� ����������, ����� - ���.
        */
        void setPrintLogicViolation(bool value);
        //! ��������� ��������� ������ ��������� � ���������� ������� (EVENT_TYPE_LOGIC_VIOLATION).
        /*! \return ���������� true, ���� ��������� � ���������� ������� ���������, ����� false.
        */
        bool getPrintLogicViolation();
        //! ��������� ������ ���������, ������� �������� ���������������� (EVENT_TYPE_WARNING).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_WARNING ����� ����������, ����� - ���.
        */
        void setPrintWarning(bool value);
        //! ��������� ��������� ������ ���������, ������� �������� ���������������� (EVENT_TYPE_WARNING).
        /*! \return ���������� true, ���� ��������� ������� �������� ����������������, ���������, ����� false.
        */
        bool getPrintWarning();
        //! ��������� ������ ����������� ��������� (EVENT_TYPE_STANDARD).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_STANDARD ����� ����������, ����� - ���.
        */
        void setPrintStandard(bool value);
        //! ��������� ��������� ������ ����������� ��������� (EVENT_TYPE_STANDARD).
        /*! \return ���������� true, ���� ����������� ��������� ���������, ����� false.
        */
        bool getPrintStandard();
        //! ��������� ������ �������������� ��������� (EVENT_TYPE_INFO).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_INFO ����� ����������, ����� - ���.
        */
        void setPrintInfo(bool value);
        //! ��������� ��������� ������ �������������� ��������� (EVENT_TYPE_INFO).
        /*! \return ���������� true, ���� �������������� ��������� ���������, ����� false.
        */
        bool getPrintInfo();
        //! ��������� ������ �������������� ��������� (EVENT_TYPE_DEBUG_INFO).
        /*! \param value ���� ����� �������� true, �� ��������� � ����� EVENT_TYPE_DEBUG_INFO ����� ����������, ����� - ���.
        */
        void setPrintDebugInfo(bool value);
        //! ��������� ��������� ������ �������������� ��������� (EVENT_TYPE_DEBUG_INFO).
        /*! \return ���������� true, ���� �������������� ��������� ���������, ����� false.
        */
        bool getPrintDebugInfo();
        //! ��������� ������ ������� � �������.
        /*! \param value ���� ����� �������� true, �� ������� ����� ���������� � �������, ����� - ���.
        */
        void setConsolePrint(bool value);
        //! ��������� ��������� ������ ������� � �������.
        /*! \return ���������� true, ���� ������� � ������� ���������, ����� false.
        */
        bool getConsolePrint();
        //! ��������� ������ ������� � ����.
        /*! \param value ���� ����� �������� true, �� ������� ����� ���������� � ����, ����� - ���.
        */
        void setFilePrint(bool value);
        //! ��������� ��������� ������ ������� � ����.
        /*! \return ���������� true, ���� ������� � ���� ���������, ����� false.
        */
        bool getFilePrint();
        //! ��������� ����� ���-�����.
        /*! \param value ��� ���-�����.
        */
        void setLogFileName(std::string value);
        //! ��������� ����� ���-�����.
        /*! \return ���������� ��� ���-�����.
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
