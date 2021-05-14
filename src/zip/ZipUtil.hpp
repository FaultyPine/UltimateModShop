/**
 * NOT MY CODE
 * https://github.com/fortheusers/libget/blob/b781f2cae3d0c39aba26bbf4581554812ee6c2d6/src/ZipUtil.hpp
 */

#pragma once

// use included minizip library
#include "minizip/zip.h"
#include "minizip/unzip.h"

#include <string>
#include <vector>
class Zip
{
public:
	Zip(const char* zipPath);
	void Close();
	~Zip();
	int AddFile(const char* internalPath, const char* path);
	int AddDir(const char* internalDir, const char* externalDir);

private:
	int Add(const char* path);
	zipFile fileToZip;
};
class UnZip
{
public:
	UnZip(const char* zipPath);
	~UnZip();
	void Close();
	int ExtractFile(const char* internalPath, const char* path);
	int ExtractAll(const char* dirToExtract);
	int ExtractDir(const char* internalDir, const char* externalDir);
	std::vector<std::string> PathDump();

private:
	int Extract(const char* path, unz_file_info_s* fileInfo);
	std::string GetFileName(unz_file_info_s* fileInfo);
	std::string GetFullFileName(unz_file_info_s* fileInfo);
	unz_file_info_s* GetFileInfo();
	unzFile fileToUnzip;
};
