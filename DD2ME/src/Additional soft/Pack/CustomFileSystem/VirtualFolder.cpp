#include "VirtualFolder.h"

#include "../Defines.h"
#include "../WorkFunctions.h"

#include <NoIrrlicht/NoIrrlicht.h>
#include <irrNet.h>

#include <fstream>

using namespace std;
using namespace CustomFileSystem;

CustomFileSystem::VirtualFolder::VirtualFolder():
    s_CryptKey(""),
    s_IsCrypted(false),
    s_IsCompressed(false)
{
    //...
}

CustomFileSystem::VirtualFolder::~VirtualFolder()
{
    clear();
}

bool CustomFileSystem::VirtualFolder::setCryptedStatus(bool crypted)
{
    if(crypted && s_CryptKey == "") return false;
    s_IsCrypted = crypted;
    return true;
}

bool CustomFileSystem::VirtualFolder::isCrypted()
{
    return s_IsCrypted;
}

void CustomFileSystem::VirtualFolder::setCryptKey(const string& crypt_key)
{
    s_CryptKey = crypt_key;
    if(s_CryptKey == "") setCryptedStatus(false);
}

string CustomFileSystem::VirtualFolder::getCryptKey()
{
    return s_CryptKey;
}

bool CustomFileSystem::VirtualFolder::setCompressedStatus(bool compressed)
{
    s_IsCompressed = compressed;
    return true;
}

bool CustomFileSystem::VirtualFolder::isCompressed()
{
    return s_IsCompressed;
}

void CustomFileSystem::VirtualFolder::clear()
{
    s_Files.clear();
}

bool CustomFileSystem::VirtualFolder::isEmpty()
{
    return s_Files.empty();
}

bool CustomFileSystem::VirtualFolder::addFile(shared_ptr<File> file)
{
    if(!file) return false;
    if(isFileExists(file->getFileName())) return false;
    s_Files[file->getFileName()] = file;
    return true;
}

bool CustomFileSystem::VirtualFolder::isFileExists(const string& filename)
{
    map<string, shared_ptr<File>>::iterator I = s_Files.find(filename);
    if ( I == s_Files.end() ) return false;
    return true;
}

bool CustomFileSystem::VirtualFolder::deleteFile(const string& filename)
{
    if(!isFileExists(filename)) return false;
    s_Files.erase(filename);
    return true;
}

shared_ptr<File> CustomFileSystem::VirtualFolder::getFile(const string& filename)
{
    if(!isFileExists(filename)) return nullptr;
    return s_Files[filename];
}

shared_ptr<File> CustomFileSystem::VirtualFolder::addFileFromFS(const string& filename, const string& result_filename)
{
    shared_ptr<File> fl = loadFileFromFS(filename, result_filename);
    if(!addFile(fl)) return nullptr;
    return fl;
}

shared_ptr<File> CustomFileSystem::VirtualFolder::loadFileFromFS(const string& filename, const string& result_filename)
{
    ifstream file_read(filename, ios::binary);
    if(!file_read) return nullptr;
    file_read.seekg(0, ios::end); // Сместили указатель чтения файла от 0-й позиции на длину файла
    size_t length = file_read.tellg(); // Вернули текущую позицию чтения (в данном случае - длину файла)
    irr::c8* buffer = new irr::c8[length]; // Создали буфер для файла
    file_read.seekg(0, ios::beg); // Вернули указатель чтения файла в начало
    file_read.read(buffer, length);
    file_read.close();

    shared_ptr<File> res_file = make_shared<File>(buffer, length, result_filename, false);
    return res_file;
}

bool CustomFileSystem::VirtualFolder::writeFileToFS(const std::string& filename, const std::string& result_filename)
{
    return writeFileToFS(getFile(filename), result_filename);
}

bool CustomFileSystem::VirtualFolder::writeFileToFS(std::shared_ptr<File> file, const std::string& result_filename)
{
    if(!file) return false;
    WorkFunctions::FileFunctions::createFoldersForFile(result_filename, STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER);
    ofstream file_write(result_filename.c_str(), ios::binary);
    if(!file_write) return false;

    file_write.write(file->getFileBuffer(), file->getFileSize());
    file_write.close();

    return true;
}

bool CustomFileSystem::VirtualFolder::load(const std::string& filename)
{
    ifstream file_read(filename.c_str(), ios::binary);
    if(!file_read) return false;
    bool is_good = true;

    file_read.seekg(0, ios::end); // Сместили указатель чтения файла от 0-й позиции на длину файла
    size_t length = file_read.tellg(); // Вернули текущую позицию чтения (в данном случае - длину файла)
    file_read.seekg(0, ios::beg); // Вернули указатель чтения файла в начало

    char buf[2];
    file_read.read(buf, 2);
    string crypt_str(1, buf[0]);
    string compress_str(1, buf[1]);

    length = length - 2; // Уменьшили на 2, так как 2 символа уже прочитали
    char* all_buffer = new char[length];
    file_read.read(all_buffer, length);
    file_read.close();

    irr::net::SInPacket all_data_packet(all_buffer, length);
    if(crypt_str == "c")
    {
        if(isCrypted()) all_data_packet.decryptPacket(s_CryptKey.c_str());
        else
        {
            cout << "Error: the file is encrypted, but the crypt mode is not enabled! File name: " << filename << endl;
            return false;
        }
    }

    string file_name = "";
    size_t file_size = 0;
    size_t number_of_files = 0;
    all_data_packet >> number_of_files;

    for(size_t i = 0; i < number_of_files; i++)
    {
        irr::net::SInPacket* file_data_packet = 0;
        all_data_packet >> file_name;
        all_data_packet >> file_size;
        char* file_buffer = new char[file_size];
        for(size_t j = 0; j < file_size; j++) all_data_packet >> file_buffer[j];
        if(compress_str == "c")
        {
            file_data_packet = new irr::net::SInPacket(file_buffer, file_size);
            file_data_packet->deCompressPacket();
            delete[] file_buffer;
            file_buffer = file_data_packet->getData();
            file_size = file_data_packet->getSize();
        }
        shared_ptr<File> res_file = make_shared<File>(file_buffer, file_size, file_name, true);
        if(!addFile(res_file)) is_good = false;
        if(file_data_packet) delete file_data_packet;
        else delete[] file_buffer;
    }
    return is_good;
}

bool CustomFileSystem::VirtualFolder::write(const std::string& filename)
{
    ofstream file_write(filename.c_str(), ios::binary);
    if(!file_write) return false;

    irr::net::SOutPacket all_data_packet;
    int number_of_files = s_Files.size();
    all_data_packet << number_of_files;
    string crypt_str = "n";
    string compress_str = "n";
    map<string, shared_ptr<File>>::iterator iter, iter1;
    for(iter = s_Files.begin(), iter1 = s_Files.end(); iter != iter1;)
    {
        irr::net::SOutPacket file_data_packet;
        char* file_buffer = iter->second->getFileBuffer();
        size_t file_size = iter->second->getFileSize();
        string file_name = iter->second->getFileName();
        if(isCompressed())
        {
            compress_str = "c";
            for(size_t i = 0; i < iter->second->getFileSize(); i++) file_data_packet << file_buffer[i];
            file_data_packet.compressPacket();
            file_buffer = file_data_packet.getData();
            file_size = file_data_packet.getSize();
        }
        all_data_packet << file_name << file_size;
        for(size_t i = 0; i < file_size; i++) all_data_packet << file_buffer[i];
        iter++;
    }
    if(isCrypted())
    {
        crypt_str = "c";
        all_data_packet.encryptPacket(s_CryptKey.c_str());
    }

    string info_str = crypt_str + compress_str;
    file_write.write( info_str.c_str(), info_str.size() );
    file_write.write(all_data_packet.getData(), all_data_packet.getSize());
    file_write.close();

    return true;
}

bool CustomFileSystem::VirtualFolder::addFolderAsRootFromFS(const std::string& foldername)
{
    bool is_good = true;
    vector<string> list_all_files = WorkFunctions::FileFunctions::getListAllFiles(foldername);
    for(size_t i = 0; i < list_all_files.size(); i++)
    {
        string res_fn = list_all_files[i];
        if(res_fn.find(foldername + STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER) == 0) res_fn = res_fn.substr( (foldername + STRING_CONSTANTS::DEFAULT_FOLDER_SPLITTER).size() );
        if(!addFileFromFS(list_all_files[i], res_fn)) is_good = false;
    }
    return is_good;
}

bool CustomFileSystem::VirtualFolder::writeAllToFolderFS(const std::string& foldername)
{
    bool is_good = true;
    map<string, shared_ptr<File>>::iterator iter, iter1;
    for(iter = s_Files.begin(), iter1 = s_Files.end(); iter != iter1;)
    {
        if(!writeFileToFS(iter->second, foldername + "/" + iter->second->getFileName())) is_good = false;
        iter++;
    }
    return is_good;
}
