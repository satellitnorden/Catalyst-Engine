#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <File/Core/File.h>

//STD.
#include <filesystem>
#include <fstream>

/*
*	File namespace, containing common code relating to files.
*/
namespace File
{

	/*
	*	Browses for a file.
	*	Returns if the action was successful.
	*/
	NO_DISCARD bool BrowseForFile(const bool save, DynamicString *const RESTRICT chosen_file, const char *const RESTRICT filter) NOEXCEPT
	{
		//TODO: Implement this!
		return false;
	}

	/*
	*	Browses for a folder.
	*/
	NO_DISCARD bool BrowseForFolder(DynamicString *const RESTRICT chosen_folder) NOEXCEPT
	{
		//TODO: Implement this!
		return false;
	}

	/*
	*	Creates a directory.
	*/
	void CreateDirectory(const char *const RESTRICT path) NOEXCEPT
	{
		std::filesystem::create_directories(path);
	}

	/*
	*	Creates a file.
	*/
	void CreateFile(const char *const RESTRICT path) NOEXCEPT
	{
		//A bit ugly, but open the file and close it.
		std::ofstream file{ path };
		file << "A file created with Linux. (:" << std::endl;
		file.close();
	}

	/*
	*	Returns if a file exists or not.
	*/
	NO_DISCARD bool Exists(const char *const RESTRICT file) NOEXCEPT
	{
		return std::filesystem::exists(file);
	}

	/*
	*	Deletes a file.
	*/
	void Delete(const char *const RESTRICT file) NOEXCEPT
	{
		std::filesystem::remove(file);
	}

	/*
	*	Returns the size of the file with the given file path.
	*/
	NO_DISCARD uint64 GetSize(const char *const RESTRICT file_path) NOEXCEPT
	{
		return std::filesystem::file_size(file_path);
	}

	/*
	*	Returns the last changed time for the given directory/file path.
	*	Expressed in seconds since epoch.
	*/
	NO_DISCARD uint64 LastChangedTime(const char *const RESTRICT directory_or_file_path) NOEXCEPT
	{
		const std::filesystem::file_time_type last_write_time{ std::filesystem::last_write_time(std::string(directory_or_file_path)) };
		const std::chrono::system_clock::duration time_since_epoch{ last_write_time.time_since_epoch() };

		return std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
	}

}
#endif