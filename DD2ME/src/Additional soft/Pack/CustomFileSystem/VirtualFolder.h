#ifndef VirtualFolder_H
#define VirtualFolder_H

#include "File.h"

#include <string>
#include <memory>
#include <map>

namespace CustomFileSystem
{
    //! ����� ��� ��������� �������-������, ��� ����� �������� �������.
    /*! ������ ����� ��������� � ������������ ������-�����, ������� ����������� ����������� �����.
    */
    class VirtualFolder
    {
        public:
            //! ����������� �����������.
            VirtualFolder();
            //! ����������� ����������.
            ~VirtualFolder();



            //! ��������� ����� �� �������� ������� � ����������� ����� ��� ������.
            /*! \param foldername ��� �����, �� ������� ���������� ������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool addFolderAsRootFromFS(const std::string& foldername);
            //! �������� � ����� ��� ����������� �����.
            /*! \param foldername ��� �����, � ������� ���������� ������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool writeAllToFolderFS(const std::string& foldername);





            //! ��������� ��� ����������� ����� �� ����� (����������� ������ � ����������).
            /*! \param filename ��� �����, �� �������� ���������� ������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool load(const std::string& filename);
            //! �������� � ���� ��� ����������� ����� (����������� ������ � ����������).
            /*! \param filename ��� �����, � ������� ���������� ������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool write(const std::string& filename);
            //! �������� ���� �� ����������� ����� � �������� �������.
            /*! \param filename ��� ����� � ����������� �����.
            \param result_filename �������������� ��� ����� � �������� �������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool writeFileToFS(const std::string& filename, const std::string& result_filename);
            //! �������� ���� � �������� �������.
            /*! \param file ������-����, ������� ������������ � �������� �������.
            \param result_filename �������������� ��� ����� � �������� �������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool writeFileToFS(std::shared_ptr<File> file, const std::string& result_filename);

            //! ��������� ���� �� �������� ������� � ������� �� ���� ���������, � ����� �������� ��� � ����������� �����.
            /*! \param filename ��� ������������ �����.
            \param result_filename �������������� ��� ����� � ����������� �����.
            \return ���������� ��������� �� ���� � ������ ������, � ��������� ������ ���������� nullptr (��������, ���� ����� � ����� ������ ���).
            */
            std::shared_ptr<File> addFileFromFS(const std::string& filename, const std::string& result_filename);
            //! ��������� ���� �� �������� ������� � ������� �� ���� ��������� ��� ���������� � ����������� �����.
            /*! \param filename ��� ������������ �����.
            \param result_filename �������������� ��� ����� � ������ �����.
            \return ���������� ��������� �� ���� � ������ ������, � ��������� ������ ���������� nullptr (��������, ���� ����� � ����� ������ ���).
            */
            std::shared_ptr<File> loadFileFromFS(const std::string& filename, const std::string& result_filename);
            //! �������� ���� �� ����������� �����.
            /*! \param filename ��� ����������� �����.
            \return ���������� ��������� �� ���� � ������ ������, � ��������� ������ ���������� nullptr (��������, ���� ����� � ����� ������ ���).
            */
            std::shared_ptr<File> getFile(const std::string& filename);
            //! ������� ���� �� ����������� �����.
            /*! \param filename ��� ���������� �����.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false (��������, ���� ����� � ����� ������ ���).
            */
            bool deleteFile(const std::string& filename);
            //! �������� ���� � ����������� �����.
            /*! \param file ����������� ����. ������ ���� ������ � ������� new � shared_ptr, ������� ��� ������ ����� ���������� �� ����� (� ������!).
            \return ���������� true � ������ ������, � ��������� ������ ���������� false (��������, ���� ���� � ����� ������ ��� ����).
            */
            bool addFile(std::shared_ptr<File> file);
            //! ���������, ����� �� ����������� �����.
            /*! \return ���������� true � ������ ������ ����������� �����, � ��������� ������ ���������� false.
            */
            bool isEmpty();
            //! ���������, ���������� �� ���� � ������.
            /*! \param filename ��� ������������ �����.
            \return ���������� true � ������ �������������, � ��������� ������ ���������� false.
            */
            bool isFileExists(const std::string& filename);
            //! �������� ��������� ����������� ����� �� ������.
            void clear();
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
            void setCryptKey(const std::string& crypt_key);
            //! �������� ���� ����������.
            std::string getCryptKey();
            //! �������� ��� ��������� ������.
            /*! \param compressed ������ ���������������� ������.
            \return ���������� true � ������ ������, � ��������� ������ ���������� false.
            */
            bool setCompressedStatus(bool compressed);
            //! �������� ������ ������.
            /*! \return ���������� true � ������ ����������� ������, � ��������� ������ ���������� false.
            */
            bool isCompressed();
        private:
            std::map<std::string, std::shared_ptr<File>> s_Files;
            std::string s_CryptKey;
            bool s_IsCrypted;
            bool s_IsCompressed;
    };
};
#endif
