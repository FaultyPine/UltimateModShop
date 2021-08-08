#include "ZipUtil.hpp"

#define PHYFSPP_IMPL
#include "physfs.hpp"

/**
 *  So im using PhysFS rn for .zip & .7z
 *  Still need something to handle .rar tho
 *     Seems like zziplib *can* actually handle rars maybe??? Nx-Shell uses it and it claims rar extraction
 *   Before I try zziplib tho I'd like to give libarchive another try possibly
 */

 
// ----------------------------------- PHYSFS --------------------------------------------
void iterateDirsAndExtract(const std::string& dirname, const std::string& dest) {
    std::vector<std::string> files = PhysFS::enumerateFiles(dirname);
    for (std::string file_name : files) {
        std::string path = dirname + file_name + "/";
        if (PHYSFS_isDirectory(path.c_str())) {
            iterateDirsAndExtract(path, dest);
        }
        else {
            path.erase(0,1); // get rid of "/" at the beginning
            if (path.back() == '/')
                path.pop_back(); // get rid of "/" at the end

            std::filesystem::path final_dest = dest + path;
            std::filesystem::path final_dest_without_filename = std::filesystem::path(dest + path).remove_filename();
            if (!std::filesystem::exists(final_dest_without_filename))
                std::filesystem::create_directories(final_dest_without_filename);

            std::fstream file;
            file.open(final_dest, std::ios::out);

            PHYSFS_File* phys_file = PhysFS::openWithMode(path.c_str(), PhysFS::mode::READ);
            PHYSFS_sint64 phys_filesize = PHYSFS_fileLength(phys_file);

            char* buf = new char[phys_filesize + 1];
            int length_read = PHYSFS_read(phys_file, buf, 1, phys_filesize);
            buf[phys_filesize+1] = 0; // null terminate...?
            
            file.write(buf, length_read+1);

            file.close();
            
            PHYSFS_close(phys_file);
            delete buf;
            
        }
    }
}

void PhysFSArchiveExtractInner(const std::string& filename, const std::string& dest) {
    PHYSFS_mount(filename.c_str(), NULL, true);

    iterateDirsAndExtract("/", dest);

    PHYSFS_unmount(filename.c_str());
}
// -------------------------------------------------------------------------------








int UnZip::ArchiveExtract(const std::string& filename, const std::string& dest) {
    if (!std::filesystem::exists(filename)) {
        brls::Logger::error("File doesn't exist! Cannot extract.");
        return 0;
    }

    std::filesystem::path p = std::filesystem::path(filename);
    std::string extension = p.extension().string();

    brls::Logger::debug("Extracting: {} -> {}   [extension: {}]", filename, dest, extension);

    // Use PhysFS
    if (extension == ".zip" || extension == ".7z") {
        if (!std::filesystem::exists(dest))
            std::filesystem::create_directories(dest);
        PhysFSArchiveExtractInner(filename, dest);
    }
    // Use ???
    else if (extension == ".rar") {
        if (!std::filesystem::exists(dest))
            std::filesystem::create_directories(dest);
        brls::Logger::warning(".rar archive extraction not supported yet");
        // ???
        return 0;
    }
    else {
        brls::Logger::error("Unsupported archive type! Cannot extract!");
        return 0;
    }
    return 1;
} 

void UnZip::PhysFSInit() {
    PhysFS::init(nullptr);
}
void UnZip::PhysFSDeinit() {
    PhysFS::deinit();
}