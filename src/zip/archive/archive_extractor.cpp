#include <malloc.h>
#include <string.h>
#include "archive_entry.h"
#include "archive.h"
#include "ums_utils.h"
#include "archive_extractor.h"

static char *Archive_RemoveFileExt(char *filename) {
	char *ret, *lastdot;

   	if (filename == NULL)
   		return NULL;
   	if ((ret = (char*)malloc(strlen(filename) + 1)) == NULL)
   		return NULL;

   	strcpy(ret, filename);
   	lastdot = strrchr(ret, '.');

   	if (lastdot != NULL)
   		*lastdot = '\0';

   	return ret;
}

typedef unsigned long int u64;
typedef signed long int s64;

static u64 Archive_CountFiles(const char *path) {
	int ret = 0;
	u64 count = 0;
	
	struct archive *handle = archive_read_new();
	ret = archive_read_support_format_all(handle);
	ret = archive_read_open_filename(handle, path, 1024);
	
	if (ret != ARCHIVE_OK) {
		brls::Logger::error("archive_read_open_filename :", ret);
		return 1;
	}
	
	for (;;) {
		struct archive_entry *entry = NULL;
		int ret = archive_read_next_header(handle, &entry);
		if (ret == ARCHIVE_EOF)
			break;
		
		count++;
	}
	
	ret = archive_read_free(handle);
	return count;
}

static int Archive_WriteData(struct archive *src, struct archive *dst) {
	int ret = 0;
	
	for (;;) {
		const void *chunk = NULL;
		size_t length = 0;
		s64 offset = 0;
		
		ret = archive_read_data_block(src, &chunk, &length, &offset);
		if (ret == ARCHIVE_EOF)
			return ARCHIVE_OK;
		
		if (ret != ARCHIVE_OK)
			return ret;
			
		ret = archive_write_data_block(dst, chunk, length, offset);
		if (ret != ARCHIVE_OK)
			return ret;
	}
	
	return -1;
}

int Archive_ExtractArchive(const char *path, const char* dest_path) {
    std::filesystem::create_directories(dest_path);
	
	int count = 0;
	u64 max = Archive_CountFiles(path);
	
	int ret = 0;
	struct archive *handle = archive_read_new();
	struct archive *dst = archive_write_disk_new();
	
	ret = archive_read_support_format_all(handle);
	ret = archive_read_open_filename(handle, path, 1024);
	if (ret != ARCHIVE_OK) {
		brls::Logger::error("archive_read_open_filename :", ret);
		return 1;
	}
	
	for (;;) {
		//brls::Logger::debug("Extracting {} {} {}", path, count, max);
		
		struct archive_entry *entry = NULL;
		ret = archive_read_next_header(handle, &entry);
		if (ret == ARCHIVE_EOF)
			break;
			
		if (ret != ARCHIVE_OK) {
			brls::Logger::error("archive_read_next_header failed: {}", ret);
			
			if (ret != ARCHIVE_WARN)
				break;
		}
		
		const char *entry_path = archive_entry_pathname(entry);
		char new_path[1024];

		ret = snprintf(new_path, sizeof(new_path), "%s/%s", dest_path, entry_path);
		ret = archive_entry_update_pathname_utf8(entry, new_path);
		if (!ret) {
			brls::Logger::error("archive_entry_update_pathname_utf8: {}", ret);
			break;
		}
		
		ret = archive_write_disk_set_options(dst, ARCHIVE_EXTRACT_UNLINK);
		ret = archive_write_header(dst, entry);
		if (ret != ARCHIVE_OK) {
			brls::Logger::error("archive_write_header failed: {}", ret);
			break;
		}
		
		ret = Archive_WriteData(handle, dst);
		ret = archive_write_finish_entry(dst);
		count++;
	}
	
	ret = archive_write_free(dst);
	ret = archive_read_free(handle);
	//free(dest_path);
	//free(dirname_without_ext);
	return ret;
}