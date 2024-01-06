#ifndef FILE_H
#define FILE_H

#include <string>

namespace CustomFileSystem
{
    //! ����� ��� �������� �����.
    class File
    {
        public:
            //! ����������� �����������.
            File();
            //! �����������.
            /*! \param data ����-������ �����. ������ ����������.
            \param length ����� ����-�������.
            \param filename ��� �����.
            \param copy_buffer ���������� ����� ��� ����� ������.
            */
            File(char* data, const size_t length, const std::string& filename, bool copy_buffer);
            //! ����������� ����������.
            ~File();
            //! �������� ������ ��� ����� (� ����).
            /*! \return ���������� ������ ��� �����.
            */
            std::string getFileName();
            //! ���������� ������ ��� ����� (� ����).
            /*! \param filename ��������������� ������ ��� �����.
            */
            void setFileName(std::string filename);
            //! �������� ������ �����.
            /*! \return ���������� ������ �����.
            */
            size_t getFileSize();
            //! �������� ��������� �� ����� �����.
            /*! \return ���������� ��������� �� ����� �����.
            */
            char* getFileBuffer();
            //! �������� ��� ����� �����.
            /*! \return ���������� ��� ����� �����.
            */
            std::string getFileFolder();
        private:
            //! ��� �����.
            std::string s_FileName;
            //! ������ �����.
            size_t s_Length;
            //! ����� �����.
            char* s_Buffer;
    };
};
#endif
