#ifndef PARSERINFOFILE_H
#define PARSERINFOFILE_H

#include <string>

#include "PostParsingStruct.h"

namespace IniParser
{
    //! ����� ��� ��������� ini-������.
    /*! ������ ����� ��������� � ������������ ini-�����, ������� ����������� ����������� PostParsingStruct.
    */
    class ParserInfoFile
    {
        public:
            //! ����������� �����������.
            ParserInfoFile();
            //! ����������� ����������.
            ~ParserInfoFile();
            //! ��������� ���������� ��������� �� �����.
            /*! \param filename ��� �����, ������� ���� ����������.
            \param prs ��������� �� PostParsingStruct, ���� ���������� ������. ���� = 0, �� �������� �����.
            \param log ����� ���� ������ � �������.
            \return ���������� ��������� �� PostParsingStruct. �� ����� NULL, ���� ��������� ������ ��������.
            */
            PostParsingStruct* getParsedFromFile(std::string filename, PostParsingStruct* prs = 0, bool log = true);
            //! ���������� ���������� ��������� �� �����.
            /*! \param prs ��������� �� PostParsingStruct, ���� �� ��������� ���������� �� �����.
            \param filename ��� �����, ������� ���� ����������.
            \param log ����� ���� ������ � �������.
            \return ���������� ��������� �� PostParsingStruct. �� ����� NULL, ���� ��������� ������ ��������.
            */
            void addParsedFromFile(PostParsingStruct* prs, std::string filename, bool log = true);
            //! ��������� ���������� ��������� �� ������.
            /*! \param str ������, ������� ���� ����������.
            \param splitter ����������� � ������ ������ �������� �� ����� ������.
            */
            PostParsingStruct* getParsedFromString(std::string str, std::string splitter);
            //! ����������� PostParsingStruct � ������.
            /*! \param pps �������������� PostParsingStruct.
            \param splitter ����������� � ������ ������ �������� �� ����� ������.
            \return ���������� ������, ������� �������� ����������� �����������.
            */
            std::string convertPostParsingStructToString(PostParsingStruct* pps, std::string splitter);
            //! ������ PostParsingStruct � ����.
            /*! \param pps ������������ PostParsingStruct.
            \param filename ��� �����, ���� ��� ������.
            \param log ����� ���� ������ � �������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool writeParsedToFile(PostParsingStruct* pps, std::string filename, bool log = true);
    };
};
#endif
