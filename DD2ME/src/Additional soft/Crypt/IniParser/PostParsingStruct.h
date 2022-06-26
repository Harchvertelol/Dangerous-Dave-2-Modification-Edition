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
            \param error_on_not_exists ���� ����� true, �� ����� �������� ������ ��� ��������� ������������� ����������, ����� ���
            \return ���������� �������� ����������.
            */
            std::string getValue(std::string A, std::string B, bool error_on_not_exists = true);
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
            //! ��������� ���� ���������� �� �����.
            /*! \param A �������� �����.
            \return ���������� map, ���������� ��� ���������� �� �����.
            */
            std::map<std::string, std::string>& getMapVariables(std::string A);
            //! ��������� ��� �������� �� ���������� ���������� � �������.
            /*! \param pps ������ �������� ��� ��������.
            */
            void addPostParsingStruct(PostParsingStruct* pps);
            //! ��������� ������������� �����.
            /*! \param A �������� �����.
            \return ���������� true, ���� ����������, ����� false.
            */
            bool isExists(std::string A);
            //! ��������� ������������� ����������.
            /*! \param A �������� �����.
            \param B �������� ����������.
            \return ���������� true, ���� ����������, ����� false.
            */
            bool isExists(std::string A, std::string B);
            //! ���������� ��� ������������ ����� ��� �������.
            /*! \param A ��� �����.
            */
            void setFileName(std::string A);
            //! ������� ����������.
            /*! \param A �������� �����.
            \param B �������� ����������.
            */
            void remove(std::string A, std::string B);
            //! ������� ����.
            /*! \param A �������� �����.
            */
            void remove(std::string A);
            //! ������� ��� ����������.
            void clear();
        private:
            std::map<std::string, std::map<std::string, std::string> > s_Variables;
            std::string s_FileName;
    };
};
#endif
