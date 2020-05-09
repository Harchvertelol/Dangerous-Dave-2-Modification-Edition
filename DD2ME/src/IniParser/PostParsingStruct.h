#ifndef POSTPARSINGSTRUCT_H
#define POSTPARSINGSTRUCT_H

#include <map>

#include <string>

namespace IniParser
{
    //! �����, ������� ����������� � ���������� ��������� ini-�����.
    /*! ������ ����� �������� ���������� � ���������� ��������� ����� � ������� ParserInfoFile.
    */
    class PostParsingStruct
    {
        public:
            //! ����������� �����������.
            PostParsingStruct();
            //! ����������� ����������.
            ~PostParsingStruct();
            //! ��������� �������� ����������.
            /*! \param A �������� �����, ������ ���� ����������.
            \param B �������� ����������.
            \return ���������� �������� ����������.
            */
            std::string getValue(std::string A, std::string B);
            //! ��������� �������� ����������.
            /*! \param A �������� �����, ��� ���� ����������.
            \param B �������� ����������.
            \param C ��������, ������� ������������� ����������.
            */
            void setValue(std::string A, std::string B, std::string C);
            //! ��������� ���� ����������.
            /*! \return ���������� map, ���������� ��� ����������.
            */
            std::map<std::string, std::map<std::string, std::string> >& getMapVariables();
            //! ��������� ��� �������� �� ���������� ���������� � �������.
            /*! \param pps ������ �������� ��� ��������.
            */
            void addPostParsingStruct(PostParsingStruct* pps);
        private:
            std::map<std::string, std::map<std::string, std::string> > s_Variables;
    };
};
#endif
