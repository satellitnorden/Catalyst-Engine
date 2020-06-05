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
	FORCE_INLINE NO_DISCARD bool BrowseForFile(const bool save, DynamicString* const RESTRICT chosen_file) NOEXCEPT
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
	FORCE_INLINE NO_DISCARD bool BrowseForFolder(DynamicString *const RESTRICT chosen_folder) NOEXCEPT
	{
		//Store the folder.
		TCHAR path[MAX_PATH];

		//Set up the browse info structure.
		BROWSEINFO bi = { 0 };
		bi.lpszTitle = { };
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lpfn = WindowsFile::BrowseForFolderCallback;
		bi.lParam = (LPARAM)"";

		const LPITEMIDLIST pidl{ SHBrowseForFolder(&bi) };

		if (pidl)
		{
			//Get the name of the chosen.
			SHGetPathFromIDList(pidl, path);

			//Free memory used.
			IMalloc *RESTRICT imalloc{ nullptr };

			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

			//Make sure the output has the correct length.
			chosen_folder->SetLength(MAX_PATH);

			//Copy the result into the output.
			for (int32 i{ 0 }; i < MAX_PATH; ++i)
			{
				(*chosen_folder)[i] = path[i];
			}

			//Succes!
			return true;
		}

		//Failure.
		return false;
	}

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
	FORCE_INLINE void Delete(const char *const RESTRICT file) NOEXCEPT
	{
		DeleteFile(file);
	}

}
#endif