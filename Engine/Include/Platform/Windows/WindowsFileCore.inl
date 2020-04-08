#if defined(CATALYST_PLATFORM_WINDOWS)
/*
*	File namespace, containing common functions relating to files.
*/
namespace File
{

	/*
	*	Creates a file.
	*/
	FORCE_INLINE void Create(const char *const RESTRICT file) NOEXCEPT
	{
		const HANDLE file_handle{	CreateFileA(	file,
									0,
									0,
									nullptr,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									nullptr) };

		CloseHandle(file_handle);
	}

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE NO_DISCARD bool Exists(const char *const RESTRICT file) NOEXCEPT
	{
		return GetFileAttributes(file) != INVALID_FILE_SIZE;
	}

}
#endif