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
            //! ���������� ���������� ��������� �� ������.
            /*! \param str_s ������, ������� ���� ����������.
            \param splitter ����������� � ������ ������ �������� �� ����� ������.
            \param prs ��������� �� PostParsingStruct, ���� �� ��������� ���������� �� ������.
            \return ���������� ��������� �� PostParsingStruct. �� ����� NULL, ���� ��������� ������ ��������.
            */
            void addParsedFromString(std::string str_s, std::string splitter, PostParsingStruct* prs);
            //! ���������� ���������� ��������� �� PostParsingStruct.
            /*! \param pps_main ��������� �� PostParsingStruct, ���� ����������� ������.
            \param pps_add ��������� �� PostParsingStruct, ������ ������� ������.
            */
            void addParsedFromPostParsingStruct(PostParsingStruct* pps_main, PostParsingStruct* pps_add);
            //! ��������� ���������� ��������� �� ������.
            /*! \param str ������, ������� ���� ����������.
            \param splitter ����������� � ������ ������ �������� �� ����� ������.
            */
            PostParsingStruct* getParsedFromString(std::string str, std::string splitter, PostParsingStruct* prs = 0, bool fixbrbn = false);
            //! ����������� PostParsingStruct � ������.
            /*! \param pps �������������� PostParsingStruct.
            \param splitter ����������� � ������ ������ �������� �� ����� ������.
            \param prs ��������� �� PostParsingStruct, ���� ���������� ������. ���� = 0, �� �������� �����.
            \param fixbrbn �������� �� � ������ ������������������ \r\n �� \n ����� ���������� (�������� ������ ���� �������� ��������).
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
            //! �������� ��� ��������� ����������.
            /*! \param crypted ������ ���������������� ����������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false (��������, ���� �� ���������� ���� ����������).
            */
            bool setCryptedStatus(bool crypted);
            //! �������� ������ ����������.
            /*! \return ���������� true � ������ ����������� ����������, � ��������� ������ ���������� false.
            */
            bool isCrypted();
            //! ���������� ���� ����������.
            /*! \param crypt_key ���� ����������. ���� ����� ������ ������, ���������� ������������� �����������.
            */
            void setCryptKey(std::string crypt_key);
            //! �������� ���� ����������.
            std::string getCryptKey();
        private:
            std::string s_CryptKey;
            bool s_IsCrypted;
    };
};
#endif
