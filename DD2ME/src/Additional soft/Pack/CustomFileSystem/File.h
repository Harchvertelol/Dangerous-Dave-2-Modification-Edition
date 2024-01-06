#ifndef FILE_H
#define FILE_H

#include <string>

namespace CustomFileSystem
{
    //! Класс для хранения файла.
    class File
    {
        public:
            //! Стандартный конструктор.
            File();
            //! Конструктор.
            /*! \param data Байт-массив файла. Массив копируется.
            \param length Длина байт-массива.
            \param filename Имя файла.
            \param copy_buffer Копировать буфер или взять данный.
            */
            File(char* data, const size_t length, const std::string& filename, bool copy_buffer);
            //! Стандартный деструктор.
            ~File();
            //! Получить полное имя файла (с путём).
            /*! \return Возвращает полное имя файла.
            */
            std::string getFileName();
            //! Установить полное имя файла (с путём).
            /*! \param filename Устанавливаемое полное имя файла.
            */
            void setFileName(std::string filename);
            //! Получить размер файла.
            /*! \return Возвращает размер файла.
            */
            size_t getFileSize();
            //! Получить указатель на буфер файла.
            /*! \return Возвращает указатель на буфер файла.
            */
            char* getFileBuffer();
            //! Получить имя папки файла.
            /*! \return Возвращает имя папки файла.
            */
            std::string getFileFolder();
        private:
            //! Имя файла.
            std::string s_FileName;
            //! Размер файла.
            size_t s_Length;
            //! Буфер файла.
            char* s_Buffer;
    };
};
#endif
