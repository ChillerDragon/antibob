#include <polybob/base/detect.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/base/system/fs.h>
#include <polybob/base/system/str.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace polybob {

void fs_listdir(const char *dir, FS_LISTDIR_CALLBACK cb, int type, void *user)
{
#if defined(CONF_FAMILY_WINDOWS)
	char buffer[IO_MAX_PATH_LENGTH];
	str_format(buffer, sizeof(buffer), "%s/*", dir);
	const std::wstring wide_buffer = windows_utf8_to_wide(buffer);

	WIN32_FIND_DATAW finddata;
	HANDLE handle = FindFirstFileW(wide_buffer.c_str(), &finddata);
	if(handle == INVALID_HANDLE_VALUE)
		return;

	do
	{
		const std::optional<std::string> current_entry = windows_wide_to_utf8(finddata.cFileName);
		if(!current_entry.has_value())
		{
			log_error("filesystem", "ERROR: file/folder name containing invalid UTF-16 found in folder '%s'", dir);
			continue;
		}
		if(cb(current_entry.value().c_str(), (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0, type, user))
			break;
	} while(FindNextFileW(handle, &finddata));

	FindClose(handle);
#else
	DIR *dir_handle = opendir(dir);
	if(dir_handle == nullptr)
		return;

	char buffer[IO_MAX_PATH_LENGTH];
	str_format(buffer, sizeof(buffer), "%s/", dir);
	size_t length = str_length(buffer);
	while(true)
	{
		struct dirent *entry = readdir(dir_handle);
		if(entry == nullptr)
			break;
		if(!str_utf8_check(entry->d_name))
		{
			log_error("filesystem", "ERROR: file/folder name containing invalid UTF-8 found in folder '%s'", dir);
			continue;
		}
		str_copy(buffer + length, entry->d_name, sizeof(buffer) - length);
		if(cb(entry->d_name, fs_is_dir(buffer), type, user))
			break;
	}

	closedir(dir_handle);
#endif
}

void fs_listdir_fileinfo(const char *dir, FS_LISTDIR_CALLBACK_FILEINFO cb, int type, void *user)
{
#if defined(CONF_FAMILY_WINDOWS)
	char buffer[IO_MAX_PATH_LENGTH];
	str_format(buffer, sizeof(buffer), "%s/*", dir);
	const std::wstring wide_buffer = windows_utf8_to_wide(buffer);

	WIN32_FIND_DATAW finddata;
	HANDLE handle = FindFirstFileW(wide_buffer.c_str(), &finddata);
	if(handle == INVALID_HANDLE_VALUE)
		return;

	do
	{
		const std::optional<std::string> current_entry = windows_wide_to_utf8(finddata.cFileName);
		if(!current_entry.has_value())
		{
			log_error("filesystem", "ERROR: file/folder name containing invalid UTF-16 found in folder '%s'", dir);
			continue;
		}

		CFsFileInfo info;
		info.m_pName = current_entry.value().c_str();
		info.m_TimeCreated = filetime_to_unixtime(&finddata.ftCreationTime);
		info.m_TimeModified = filetime_to_unixtime(&finddata.ftLastWriteTime);

		if(cb(&info, (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0, type, user))
			break;
	} while(FindNextFileW(handle, &finddata));

	FindClose(handle);
#else
	DIR *dir_handle = opendir(dir);
	if(dir_handle == nullptr)
		return;

	char buffer[IO_MAX_PATH_LENGTH];
	str_format(buffer, sizeof(buffer), "%s/", dir);
	size_t length = str_length(buffer);

	while(true)
	{
		struct dirent *entry = readdir(dir_handle);
		if(entry == nullptr)
			break;
		if(!str_utf8_check(entry->d_name))
		{
			log_error("filesystem", "ERROR: file/folder name containing invalid UTF-8 found in folder '%s'", dir);
			continue;
		}
		str_copy(buffer + length, entry->d_name, sizeof(buffer) - length);
		time_t created = -1, modified = -1;
		fs_file_time(buffer, &created, &modified);

		CFsFileInfo info;
		info.m_pName = entry->d_name;
		info.m_TimeCreated = created;
		info.m_TimeModified = modified;

		if(cb(&info, fs_is_dir(buffer), type, user))
			break;
	}

	closedir(dir_handle);
#endif
}

int fs_storage_path(const char *appname, char *path, int max)
{
#if defined(CONF_FAMILY_WINDOWS)
	WCHAR *wide_home = _wgetenv(L"APPDATA");
	if(!wide_home)
	{
		path[0] = '\0';
		return -1;
	}
	const std::optional<std::string> home = windows_wide_to_utf8(wide_home);
	if(!home.has_value())
	{
		log_error("filesystem", "ERROR: the APPDATA environment variable contains invalid UTF-16");
		path[0] = '\0';
		return -1;
	}
	str_format(path, max, "%s/%s", home.value().c_str(), appname);
	return 0;
#else
	char *home = getenv("HOME");
	if(!home)
	{
		path[0] = '\0';
		return -1;
	}

	if(!str_utf8_check(home))
	{
		log_error("filesystem", "ERROR: the HOME environment variable contains invalid UTF-8");
		path[0] = '\0';
		return -1;
	}

#if defined(CONF_PLATFORM_HAIKU)
	str_format(path, max, "%s/config/settings/%s", home, appname);
#elif defined(CONF_PLATFORM_MACOS)
	str_format(path, max, "%s/Library/Application Support/%s", home, appname);
#else
	if(str_comp(appname, "Teeworlds") == 0)
	{
		// fallback for old directory for Teeworlds compatibility
		str_format(path, max, "%s/.%s", home, appname);
	}
	else
	{
		char *data_home = getenv("XDG_DATA_HOME");
		if(data_home)
		{
			if(!str_utf8_check(data_home))
			{
				log_error("filesystem", "ERROR: the XDG_DATA_HOME environment variable contains invalid UTF-8");
				path[0] = '\0';
				return -1;
			}
			str_format(path, max, "%s/%s", data_home, appname);
		}
		else
			str_format(path, max, "%s/.local/share/%s", home, appname);
	}
	for(int i = str_length(path) - str_length(appname); path[i]; i++)
		path[i] = tolower((unsigned char)path[i]);
#endif

	return 0;
#endif
}

int fs_makedir_rec_for(const char *path)
{
	char buffer[IO_MAX_PATH_LENGTH];
	str_copy(buffer, path);
	for(int index = 1; buffer[index] != '\0'; ++index)
	{
		// Do not try to create folder for drive letters on Windows,
		// as this is not necessary and may fail for system drives.
		if((buffer[index] == '/' || buffer[index] == '\\') && buffer[index + 1] != '\0' && buffer[index - 1] != ':')
		{
			buffer[index] = '\0';
			if(fs_makedir(buffer) < 0)
			{
				return -1;
			}
			buffer[index] = '/';
		}
	}
	return 0;
}

int fs_makedir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	if(CreateDirectoryW(wide_path.c_str(), nullptr) != 0)
		return 0;
	if(GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;
	return -1;
#else
#ifdef CONF_PLATFORM_HAIKU
	struct stat st;
	if(stat(path, &st) == 0)
		return 0;
#endif
	if(mkdir(path, 0755) == 0)
		return 0;
	if(errno == EEXIST)
		return 0;
	return -1;
#endif
}

int fs_removedir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	if(RemoveDirectoryW(wide_path.c_str()) != 0)
		return 0;
	return -1;
#else
	if(rmdir(path) == 0)
		return 0;
	return -1;
#endif
}

int fs_is_file(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	DWORD attributes = GetFileAttributesW(wide_path.c_str());
	return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
#else
	struct stat sb;
	if(stat(path, &sb) == -1)
		return 0;
	return S_ISREG(sb.st_mode) ? 1 : 0;
#endif
}

int fs_is_dir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	DWORD attributes = GetFileAttributesW(wide_path.c_str());
	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
#else
	struct stat sb;
	if(stat(path, &sb) == -1)
		return 0;
	return S_ISDIR(sb.st_mode) ? 1 : 0;
#endif
}

int fs_is_relative_path(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	return PathIsRelativeW(wide_path.c_str()) ? 1 : 0;
#else
	return path[0] == '/' ? 0 : 1; // yes, it's that simple
#endif
}

int fs_chdir(const char *path)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_path = windows_utf8_to_wide(path);
	return SetCurrentDirectoryW(wide_path.c_str()) != 0 ? 0 : 1;
#else
	return chdir(path) ? 1 : 0;
#endif
}

char *fs_getcwd(char *buffer, int buffer_size)
{
#if defined(CONF_FAMILY_WINDOWS)
	const DWORD size_needed = GetCurrentDirectoryW(0, nullptr);
	std::wstring wide_current_dir(size_needed, L'0');
	dbg_assert(GetCurrentDirectoryW(size_needed, wide_current_dir.data()) == size_needed - 1, "GetCurrentDirectoryW failure");
	const std::optional<std::string> current_dir = windows_wide_to_utf8(wide_current_dir.c_str());
	if(!current_dir.has_value())
	{
		buffer[0] = '\0';
		return nullptr;
	}
	str_copy(buffer, current_dir.value().c_str(), buffer_size);
	return buffer;
#else
	char *result = getcwd(buffer, buffer_size);
	if(result == nullptr || !str_utf8_check(result))
	{
		buffer[0] = '\0';
		return nullptr;
	}
	return result;
#endif
}

const char *fs_filename(const char *path)
{
	for(const char *filename = path + str_length(path); filename >= path; --filename)
	{
		if(filename[0] == '/' || filename[0] == '\\')
			return filename + 1;
	}
	return path;
}

void fs_split_file_extension(const char *filename, char *name, size_t name_size, char *extension, size_t extension_size)
{
	dbg_assert(name != nullptr || extension != nullptr, "name or extension parameter required");
	dbg_assert(name == nullptr || name_size > 0, "name_size invalid");
	dbg_assert(extension == nullptr || extension_size > 0, "extension_size invalid");

	const char *last_dot = str_rchr(filename, '.');
	if(last_dot == nullptr || last_dot == filename)
	{
		if(extension != nullptr)
			extension[0] = '\0';
		if(name != nullptr)
			str_copy(name, filename, name_size);
	}
	else
	{
		if(extension != nullptr)
			str_copy(extension, last_dot + 1, extension_size);
		if(name != nullptr)
			str_truncate(name, name_size, filename, last_dot - filename);
	}
}

int fs_parent_dir(char *path)
{
	char *parent = nullptr;
	for(; *path; ++path)
	{
		if(*path == '/' || *path == '\\')
			parent = path;
	}

	if(parent)
	{
		*parent = 0;
		return 0;
	}
	return 1;
}

int fs_remove(const char *filename)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_filename = windows_utf8_to_wide(filename);
	return DeleteFileW(wide_filename.c_str()) == 0;
#else
	return unlink(filename) != 0;
#endif
}

int fs_rename(const char *oldname, const char *newname)
{
#if defined(CONF_FAMILY_WINDOWS)
	const std::wstring wide_oldname = windows_utf8_to_wide(oldname);
	const std::wstring wide_newname = windows_utf8_to_wide(newname);
	if(MoveFileExW(wide_oldname.c_str(), wide_newname.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH) == 0)
		return 1;
#else
	if(rename(oldname, newname) != 0)
		return 1;
#endif
	return 0;
}

int fs_file_time(const char *name, time_t *created, time_t *modified)
{
#if defined(CONF_FAMILY_WINDOWS)
	WIN32_FIND_DATAW finddata;
	const std::wstring wide_name = windows_utf8_to_wide(name);
	HANDLE handle = FindFirstFileW(wide_name.c_str(), &finddata);
	if(handle == INVALID_HANDLE_VALUE)
		return 1;

	*created = filetime_to_unixtime(&finddata.ftCreationTime);
	*modified = filetime_to_unixtime(&finddata.ftLastWriteTime);
	FindClose(handle);
#elif defined(CONF_FAMILY_UNIX)
	struct stat sb;
	if(stat(name, &sb))
		return 1;

	*created = sb.st_ctime;
	*modified = sb.st_mtime;
#else
#error not implemented
#endif

	return 0;
}

} // namespace polybob
