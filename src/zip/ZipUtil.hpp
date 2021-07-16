#pragma once

#include "ums_utils.h"


namespace UnZip
{
	/// filename is the location of the zip to extract
	/// dest is the location where the extracted files should go
	/// EX:
	/// "pc_tmp/lucas_c05_royal_guard.zip"   <- zip to extract
	/// "pc_tmp/ultimate/"					 <- everything inside above zip go in here
	void ArchiveExtract(const std::string& filename, const std::string& dest);

	void PhysFSInit();
	void PhysFSDeinit();

};
