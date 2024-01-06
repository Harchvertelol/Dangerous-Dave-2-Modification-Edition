#ifndef VirtualFolder_H
#define VirtualFolder_H

#include "File.h"

#include <string>
#include <memory>
#include <map>

namespace CustomFileSystem
{
    //! Класс для обработки пакетов-файлов, как папок файловой системы.
    /*! Данный класс загружает и обрабатывает пакеты-файлы, выдавая результатом виртуальную папку.
    */
    class VirtualFolder
    {
        public:
            //! Стандартный конструктор.
            VirtualFolder();
            //! Стандартный деструктор.
            ~VirtualFolder();



            //! Прочитать папку из файловой системы в виртуальную папку как корень.
            /*! \param foldername Имя папки, из которой происходит чтение.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool addFolderAsRootFromFS(const std::string& foldername);
            //! Записать в папку всю виртуальную папку.
            /*! \param foldername Имя папки, в которую происходит запись.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool writeAllToFolderFS(const std::string& foldername);





            //! Прочитать всю виртуальную папку из файла (учитывается сжатие и шифрование).
            /*! \param filename Имя файла, из которого происходит чтение.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool load(const std::string& filename);
            //! Записать в файл всю виртуальную папку (учитывается сжатие и шифрование).
            /*! \param filename Имя файла, в который происходит запись.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool write(const std::string& filename);
            //! Записать файл из виртуальной папки в файловою систему.
            /*! \param filename Имя файла в виртуальной папке.
            \param result_filename Результирующее имя файла в файловой системе.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool writeFileToFS(const std::string& filename, const std::string& result_filename);
            //! Записать файл в файловою систему.
            /*! \param file Объект-файл, который записывается в файловую систему.
            \param result_filename Результирующее имя файла в файловой системе.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool writeFileToFS(std::shared_ptr<File> file, const std::string& result_filename);

            //! Загрузить файл из файловой системы и вернуть на него указатель, а также добавить его в виртуальную папку.
            /*! \param filename Имя загружаемого файла.
            \param result_filename Результирующее имя файла в виртуальной папке.
            \return Возвращает указатель на файл в случае успеха, в противном случае возвращает nullptr (например, если файла с таким именем нет).
            */
            std::shared_ptr<File> addFileFromFS(const std::string& filename, const std::string& result_filename);
            //! Загрузить файл из файловой системы и вернуть на него указатель без добавления в виртуальную папку.
            /*! \param filename Имя загружаемого файла.
            \param result_filename Результирующее имя файла в классе файла.
            \return Возвращает указатель на файл в случае успеха, в противном случае возвращает nullptr (например, если файла с таким именем нет).
            */
            std::shared_ptr<File> loadFileFromFS(const std::string& filename, const std::string& result_filename);
            //! Получить файл из виртуальной папки.
            /*! \param filename Имя получаемого файла.
            \return Возвращает указатель на файл в случае успеха, в противном случае возвращает nullptr (например, если файла с таким именем нет).
            */
            std::shared_ptr<File> getFile(const std::string& filename);
            //! Удалить файл из виртуальной папки.
            /*! \param filename Имя удаляемого файла.
            \return Возвращает true в случае успеха, в противном случае возвращает false (например, если файла с таким именем нет).
            */
            bool deleteFile(const std::string& filename);
            //! Добавить файл в виртуальную папку.
            /*! \param file Добавляемый файл. Должен быть создан с помощью new и shared_ptr, удалять сам объект после добавления не нужно (и нельзя!).
            \return Возвращает true в случае успеха, в противном случае возвращает false (например, если файл с таким именем уже есть).
            */
            bool addFile(std::shared_ptr<File> file);
            //! Проверить, пуста ли виртуальная папка.
            /*! \return Возвращает true в случае пустой виртуальной папки, в противном случае возвращает false.
            */
            bool isEmpty();
            //! Проверить, существует ли файл с именем.
            /*! \param filename Имя проверяемого файла.
            \return Возвращает true в случае существования, в противном случае возвращает false.
            */
            bool isFileExists(const std::string& filename);
            //! Очистить полностью виртуальную папку от файлов.
            void clear();
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
            void setCryptKey(const std::string& crypt_key);
            //! Получить ключ шифрования.
            std::string getCryptKey();
            //! Включить или выключить сжатие.
            /*! \param compressed Статус устанавливаемого сжатия.
            \return Возвращает true в случае успеха, в противном случае возвращает false.
            */
            bool setCompressedStatus(bool compressed);
            //! Получить статус сжатия.
            /*! \return Возвращает true в случае включённого сжатия, в противном случае возвращает false.
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
