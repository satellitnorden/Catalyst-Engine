#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>

//STL.
#include <filesystem>

class ContentCache final
{

public:

	//The current version.
	constexpr static uint64 CURRENT_VERSION{ 1 };

	/*
	*	Entry class definition.
	*/
	class Entry final
	{

	public:

		//The identifier.
		uint64 _Identifier;

		//The version.
		uint64 _Version;

		//The last write time.
		std::filesystem::file_time_type _LastWriteTime;

	};

	//The version.
	uint64 _Version;

	//The entries.
	DynamicArray<Entry> _Entries;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ContentCache(const char *const RESTRICT file_path) NOEXCEPT
	{
		if (!File::Exists(file_path))
		{
			return;
		}

		//Read the file.
		BinaryInputFile file{ file_path };

		//Read the version.
		uint64 version;
		file.Read(&version, sizeof(uint64));

		//If this is not the current version, disregard.
		if (version != CURRENT_VERSION)
		{
			file.Close();

			return;
		}

		//Read the number of entries.
		uint64 number_of_entries;
		file.Read(&number_of_entries, sizeof(uint64));

		_Entries.Upsize<false>(number_of_entries);

		//Read the entries.
		for (Entry &entry : _Entries)
		{
			file.Read(&entry, sizeof(Entry));
		}

		//Close the file.
		file.Close();
	}

	/*
	*	Returns if the given identifier with the given last write time needs a recompile.
	*/
	FORCE_INLINE NO_DISCARD bool NeedsRecompile(const uint64 identifier, const uint64 version, const std::filesystem::file_time_type last_write_time) NOEXCEPT
	{
#if 0
		return true;
#else
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				if (entry._Version != version)
				{
					return true;
				}

				else if (entry._LastWriteTime < last_write_time)
				{
					return true;
				}

				else
				{
					return false;
				}
			}
		}

		_Entries.Emplace();
		Entry &new_entry{ _Entries.Back() };

		new_entry._Identifier = identifier;
		new_entry._Version = version;
		new_entry._LastWriteTime = last_write_time;

		return true;
#endif
	}

	/*
	*	Updates the entry with the given identifier.
	*/
	FORCE_INLINE void UpdateEntry(const uint64 identifier, const uint64 version, const std::filesystem::file_time_type last_write_time) NOEXCEPT
	{
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				entry._Version = version;
				entry._LastWriteTime = last_write_time;

				break;
			}
		}
	}

	/*
	*	Writes this shader cache.
	*/
	FORCE_INLINE void Write(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Open the file.
		BinaryOutputFile file{ file_path };

		//Write the version.
		const uint64 version{ CURRENT_VERSION };
		file.Write(&version, sizeof(uint64));

		//Write the number of entries.
		const uint64 number_of_entries{ _Entries.Size() };
		file.Write(&number_of_entries, sizeof(uint64));

		//Write the entries.
		for (const Entry &entry : _Entries)
		{
			file.Write(&entry, sizeof(Entry));
		}

		//Close the file.
		file.Close();
	}

};