#if defined(CATALYST_PLATFORM_WINDOWS)
/*
*	Windows file namespace, for Windows specific functionality.
*/
namespace WindowsFile
{

	/*
	*	The browse for folder callback.
	*/
	static int32 CALLBACK BrowseForFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		if (uMsg == BFFM_INITIALIZED)
		{
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		}

		return 0;
	}

}

/*
*	File namespace, containing common code relating to files.
*/
namespace File
{

	/*
	*	Browses for a file.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFile(const bool save, DynamicString* const RESTRICT chosen_file, const char *const RESTRICT filter) NOEXCEPT
	{
		bool success{ false };

		IFileDialog *RESTRICT file_dialog;
		
		if (SUCCEEDED(::CoCreateInstance(save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, save ? IID_IFileSaveDialog : IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&file_dialog))))
		{
			//Set the title.
			file_dialog->SetTitle(L"Choose A File:");

			//Don't allow files that doesn't exist if this isn't a save action.
			if (!save)
			{
				DWORD current_options{ 0 };
				file_dialog->GetOptions(&current_options);
				file_dialog->SetOptions(current_options | FOS_FILEMUSTEXIST);
			}

			//Set the filer.
			if (filter)
			{

				WCHAR windows_filter[MAX_PATH];

				{
					const uint64 string_length{ strlen(filter) };

					for (uint64 i{ 0 }; i < string_length; ++i)
					{
						windows_filter[i] = filter[i];
					}

					windows_filter[string_length] = '\0';
				}

				COMDLG_FILTERSPEC filter_specification;

				filter_specification.pszName = L"Filter";
				filter_specification.pszSpec = windows_filter;

				file_dialog->SetFileTypes(1, &filter_specification);
			}

			//Show the file dialog.
			if (SUCCEEDED(file_dialog->Show(nullptr)))
			{
				IShellItem *RESTRICT result;
				if (SUCCEEDED(file_dialog->GetResult(&result)))
				{
					PWSTR file_path{ nullptr };

					if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &file_path)))
					{
						char converted_file_path[MAX_PATH];

						for (uint16 i{ 0 }; i < MAX_PATH; ++i)
						{
							converted_file_path[i] = static_cast<char>(file_path[i]);

							if (!file_path[i])
							{
								break;
							}
						}

						*chosen_file = converted_file_path;

						::CoTaskMemFree(file_path);

						success = true;
					}
				}
			}
		}

		return success;
	}

	/*
	*	Browses for a folder.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFolder(DynamicString *const RESTRICT chosen_folder) NOEXCEPT
	{
		bool success{ false };

		IFileDialog *RESTRICT file_dialog;

		if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&file_dialog))))
		{
			//Set the title.
			file_dialog->SetTitle(L"Choose A File:");

			//Set options.
			DWORD current_options{ 0 };
			file_dialog->GetOptions(&current_options);
			file_dialog->SetOptions(current_options | FOS_PICKFOLDERS);

			//Show the file dialog.
			if (SUCCEEDED(file_dialog->Show(nullptr)))
			{
				IShellItem *RESTRICT result;

				if (SUCCEEDED(file_dialog->GetResult(&result)))
				{
					PWSTR file_path{ nullptr };

					if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &file_path)))
					{
						char converted_file_path[MAX_PATH];

						for (uint16 i{ 0 }; i < MAX_PATH; ++i)
						{
							converted_file_path[i] = static_cast<char>(file_path[i]);

							if (!file_path[i])
							{
								break;
							}
						}

						*chosen_folder = converted_file_path;

						::CoTaskMemFree(file_path);

						success = true;
					}
				}
			}
		}

		return success;
	}

	/*
	*	Creates a directory.
	*/
	FORCE_INLINE static void CreateDirectory(const char* const RESTRICT path) NOEXCEPT
	{
		CreateDirectoryA(path, nullptr);
	}

	/*
	*	Creates a file.
	*/
	FORCE_INLINE static void CreateFile(const char *const RESTRICT path) NOEXCEPT
	{
		const HANDLE file_handle{	CreateFileA(path,
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
	FORCE_INLINE static NO_DISCARD bool Exists(const char *const RESTRICT file) NOEXCEPT
	{
		TCHAR windows_path[MAX_PATH];

		const uint64 string_length{ strlen(file) };

		for (uint64 i{ 0 }; i < string_length; ++i)
		{
			windows_path[i] = file[i];
		}

		windows_path[string_length] = '\0';

		return GetFileAttributes(windows_path) != INVALID_FILE_SIZE;
	}
	
	/*
	*	Deletes a file.
	*/
	FORCE_INLINE static void Delete(const char *const RESTRICT file) NOEXCEPT
	{
		DeleteFileA(file);
	}

	/*
	*	Returns the size of the file with the given file path.
	*/
	FORCE_INLINE static NO_DISCARD uint64 GetSize(const char* const RESTRICT file_path) NOEXCEPT
	{
		HANDLE file_handle{ CreateFileA(file_path,
										GENERIC_READ,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										nullptr,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,
										nullptr) };
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			ASSERT(false, "File::GetSize() failed!");

			return 0;
		}

		LARGE_INTEGER file_size;

		if (!GetFileSizeEx(file_handle, &file_size))
		{
			CloseHandle(file_handle);

			ASSERT(false, "File::GetSize() failed!");

			return 0;
		}

		CloseHandle(file_handle);

		return static_cast<uint64>(file_size.QuadPart);
	}

}
#endif