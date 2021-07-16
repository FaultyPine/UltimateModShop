#pragma once
#include <zzip/zzip.h>

#include "ums_utils.h"

#define FS_MAX_PATH 200

namespace ArchiveHelper {
    std::string ConstructPath(const char path[FS_MAX_PATH]) {
        //std::string new_path = cfg.cwd;
        //new_path.append("/");
        //new_path.append(std::filesystem::path(path).stem());
        //new_path.append("/");
        return std::filesystem::path(path); //     /mnt/c/Users/gclar/Downloads
    }
    
    std::string ConstructDirname(const char path[FS_MAX_PATH], char *dirname) {
        std::string new_dirname = ArchiveHelper::ConstructPath(path);
        new_dirname.append(std::filesystem::path(dirname).parent_path());
        return new_dirname;
    }
    
    std::string ConstructFilename(const char path[FS_MAX_PATH], char *filename) {
        std::string new_filename = ArchiveHelper::ConstructPath(path);
        new_filename.append(filename);
        return new_filename;
    }
    
    int RecursiveMakeDir(const std::string &path) {
        int ret = 1;
        std::filesystem::create_directories(path);
        return ret;
    }
    
    int ExtractFile(ZZIP_DIR *dir, const ZZIP_DIRENT &entry, const std::string &path) {
        int ret = 0;
        ZZIP_FILE *src_handle = zzip_file_open(dir, entry.d_name, O_RDONLY);
        if (!src_handle) {
            //Log::Error("zzip_file_open(%s) failed\n", path.c_str());
            return -1;
        }
            
        char dest_path[FS_MAX_PATH + 1];
        std::snprintf(dest_path, FS_MAX_PATH, path.c_str());
        
        //if (!std::filesystem::exists(dest_path))

            //fsFsCreateFile(fs, dest_path, entry.st_size, 0);
            
        //FsFile dest_handle;
        std::fstream dest_handle;
        /*if (R_FAILED(ret = fsFsOpenFile(fs, dest_path, FsOpenMode_Write, &dest_handle))) {
            //Log::Error("fsFsOpenFile(%s) failed: 0x%x\n", path.c_str(), ret);
            zzip_file_close(src_handle);
            return ret;
        }*/
        dest_handle.open(dest_path, std::ios::out);
        
        const unsigned long long buf_size = 0x10000;
        //s64 offset = 0;
        unsigned char *buf = new unsigned char[buf_size];
        
        zzip_ssize_t bytes_read = 0;
        std::string filename = std::filesystem::path(entry.d_name).filename();
        while (0 < (bytes_read = zzip_read(src_handle, buf, buf_size - 1))) {
            /*if (R_FAILED(ret = fsFileWrite(&dest_handle, offset, buf, bytes_read, FsWriteOption_Flush))) {
                //Log::Error("fsFileWrite(%s) failed: 0x%x\n", path.c_str(), ret);
                delete[] buf;
                zzip_file_close(src_handle);
                fsFileClose(&dest_handle);
                return ret;
            }*/
            dest_handle.write((const char*)buf, bytes_read);
            
            //offset += bytes_read;
            std::memset(buf, 0, buf_size);
            //Popups::ProgressPopup(static_cast<float>(offset), static_cast<float>(entry.st_size), strings[cfg.lang][Lang::ArchiveExtracting], filename.c_str());
        }
        
        delete[] buf;
        //fsFileClose(&dest_handle);
        dest_handle.close();
        zzip_file_close(src_handle);
        return 0; 
    }
    
    void Extract(const char path[FS_MAX_PATH]) {
        ZZIP_DIR *dir;
        ZZIP_DIRENT entry;
        zzip_error_t error;
        
        dir = zzip_dir_open(path, &error);
        if (!dir) {
            //Log::Error("zzip_dir_open(%s) failed: 0x%x\n", path, error);
            return;
        }
            
        while (zzip_dir_read(dir, &entry)) {
            std::string pathname = ArchiveHelper::ConstructDirname(path, entry.d_name);
            //ArchiveHelper::RecursiveMakeDir(pathname);
            std::filesystem::create_directories(pathname);
            
            std::string filename = ArchiveHelper::ConstructFilename(path, entry.d_name);
            ArchiveHelper::ExtractFile(dir, entry, filename);
        }
        
        zzip_dir_close(dir);
    }
}