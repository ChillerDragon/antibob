#pragma once

#include <polybob/base/system.h>

namespace polybob
{

	void fs_listdir(const char *dir, FS_LISTDIR_CALLBACK cb, int type, void *user);
	void fs_listdir_fileinfo(const char *dir, FS_LISTDIR_CALLBACK_FILEINFO cb, int type, void *user);
	int fs_storage_path(const char *appname, char *path, int max);
	int fs_makedir_rec_for(const char *path);
	int fs_makedir(const char *path);
	int fs_removedir(const char *path);
	int fs_is_file(const char *path);
	int fs_is_dir(const char *path);
	int fs_is_relative_path(const char *path);
	int fs_chdir(const char *path);
	char *fs_getcwd(char *buffer, int buffer_size);
	const char *fs_filename(const char *path);
	void fs_split_file_extension(const char *filename, char *name, size_t name_size, char *extension = nullptr, size_t extension_size = 0);
	int fs_parent_dir(char *path);
	int fs_remove(const char *filename);
	int fs_rename(const char *oldname, const char *newname);
	int fs_file_time(const char *name, time_t *created, time_t *modified);

} // namespace polybob
