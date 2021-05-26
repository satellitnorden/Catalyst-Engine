#if defined(CATALYST_PLATFORM_ANDROID)
/*
*	File namespace, containing common code relating to files.
*/
namespace File
{

	/*
	*	Browses for a file.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFile(const bool save, DynamicString* const RESTRICT chosen_file) NOEXCEPT
	{
		return false;
	}

	/*
	*	Browses for a folder.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFolder(DynamicString *const RESTRICT chosen_folder) NOEXCEPT
	{
		return false;
	}

	/*
	*	Creates a directory.
	*/
	FORCE_INLINE static void CreateDirectory(const char* const RESTRICT path) NOEXCEPT
	{
		std::filesystem::create_directory(path);
	}

	/*
	*	Creates a file.
	*/
	FORCE_INLINE static void CreateFile(const char *const RESTRICT path) NOEXCEPT
	{
		std::filesystem::create_directory(path);
	}

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE static NO_DISCARD bool Exists(const char *const RESTRICT file) NOEXCEPT
	{
		return std::filesystem::exists(file);
	}
	
	/*
	*	Deletes a file.
	*/
	FORCE_INLINE static void Delete(const char *const RESTRICT file) NOEXCEPT
	{
		std::filesystem::remove(file);
	}

	/*
	*	Returns the size of the file with the given file path.
	*/
	FORCE_INLINE static NO_DISCARD uint64 GetSize(const char* const RESTRICT file_path) NOEXCEPT
	{
		return std::filesystem::file_size(file_path);
	}

}
#endif