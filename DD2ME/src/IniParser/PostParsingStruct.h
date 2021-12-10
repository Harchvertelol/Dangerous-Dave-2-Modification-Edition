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
            */
            bool isExists(std::string A);
            //! ��������� ������������� ����������.
            /*! \param A �������� �����.
            \param B �������� ����������.
            */
            bool isExists(std::string A, std::string B);
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
    };
};
#endif
