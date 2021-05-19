#pragma once

#include "ums_utils.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <zip/archive.h>
#include <zip/archive_entry.h>
#include <fcntl.h>
#include <unistd.h>

namespace UnZip
{
	/// filename is the location of the zip to extract
	/// dest is the location where the extracted files should go
	/// EX:
	/// "pc_tmp/lucas_c05_royal_guard.zip"   <- zip to extract
	/// "pc_tmp/ultimate/"					 <- everything inside above zip go in here
	int ArchiveExtract(std::string filename, std::string dest);

};
