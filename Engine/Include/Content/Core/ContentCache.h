#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/StaticString.h>

//File.
#include <File/Core/File.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STD.
#include <fstream>

class ContentCache final
{

public:

	/*
	*	Entry class definition.
	*/
	class Entry final
	{

	public:

		//The file path.
		StaticString<MAXIMUM_FILE_PATH_LENGTH> _FilePath;

		//The identifier.
		uint64 _Identifier;

		//The version.
		uint64 _Version;

		//The last changed time.
		uint64 _LastChangedTime;

		/*
		*	Returns if this entry needs a compile.
		*/
		FORCE_INLINE NO_DISCARD bool NeedsCompile(const uint64 current_version) const NOEXCEPT
		{
			//If the identifier is invalid, it needs a compile.
			if (_Version == UINT64_MAXIMUM)
			{
				return true;
			}

			//If the last changed time is invalid, it needs a compile.
			if (_LastChangedTime == UINT64_MAXIMUM)
			{
				return true;
			}

			//If the stored version differs, it needs a compile.
			if (_Version != current_version)
			{
				return true;
			}

			//If the last changed time version differs, it needs a compile.
			if (_LastChangedTime != File::LastChangedTime(_FilePath.Data()))
			{
				return true;
			}

			//Otherwise, it doesn't need a compile!
			return false;
		}

		/*
		*	Updates this entry.
		*/
		FORCE_INLINE void Update(const uint64 current_version) NOEXCEPT
		{
			//Set the version.
			_Version = current_version;

			//Set the last changed time.
			_LastChangedTime = File::LastChangedTime(_FilePath.Data());
		}

	};

	/*
	*	Reads this content cache from the given file path.
	*/
	FORCE_INLINE void Read(const char *const RESTRICT file_path) NOEXCEPT
	{
		//If the file doesn't exist, abort.
		if (!File::Exists(file_path))
		{
			return;
		}

		//Read the JSON.
		nlohmann::ordered_json json;

		{
			//Open the file.
			std::ifstream file{ file_path };

			//Read the JSON.
			json << file;

			//Close the file.
			file.close();
		}

		//Read the version.
		const uint64 version{ json["Version"].get<uint64>() };

		//Abort if it's not the correct version.
		if (version != static_cast<uint64>(Version::CURRENT_VERSION))
		{
			return;
		}

		//Read the entries.
		const nlohmann::ordered_json &entries_entry{ json["Entries"] };

		for (const nlohmann::ordered_json &entry_entry : entries_entry)
		{
			//Add the new entry.
			_Entries.Emplace();
			Entry &new_entry{ _Entries.Back() };

			//Read the file path.
			new_entry._FilePath = entry_entry["FilePath"].get<std::string>().c_str();

			//Read the identifier.
			new_entry._Identifier = entry_entry["Identifier"].get<uint64>();

			//Read the version.
			new_entry._Version = entry_entry["Version"].get<uint64>();

			//Read the last changed time.
			new_entry._LastChangedTime = entry_entry["LastChangedTime"].get<uint64>();
		}
	}

	/*
	*	Returns the entry with the given file path.
	*/
	FORCE_INLINE NO_DISCARD Entry *const RESTRICT GetEntry(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Calculate the identifier.
		const uint64 identifier{ HashAlgorithms::MurmurHash64(file_path, StringUtilities::StringLength(file_path)) };

		//Check if we have an already existing entry.
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				return &entry;
			}
		}

		//Otherwise, add a new one and return that one.
		_Entries.Emplace();
		Entry &entry{ _Entries.Back() };

		//Set the file path.
		entry._FilePath = file_path;

		//Set the identifier.
		entry._Identifier = identifier;

		//Set the version.
		entry._Version = UINT64_MAXIMUM;

		//Set the last changed time.
		entry._LastChangedTime = UINT64_MAXIMUM;

		//Return the new entry!
		return &entry;
	}

	/*
	*	Writes this shader cache.
	*/
	FORCE_INLINE void Write(const char* const RESTRICT file_path) NOEXCEPT
	{
		//Set up the JSON.
		nlohmann::ordered_json json;

		//Write the version.
		json["Version"] = static_cast<uint64>(Version::CURRENT_VERSION);

		//Write the entries.
		nlohmann::ordered_json &entries_entry{ json["Entries"] };

		for (const Entry &entry : _Entries)
		{
			//Set up the entry entry.
			nlohmann::ordered_json entry_entry;

			//Set the file path.
			entry_entry["FilePath"] = entry._FilePath.Data();

			//Set the identifier.
			entry_entry["Identifier"] = entry._Identifier;

			//Set the version.
			entry_entry["Version"] = entry._Version;

			//Set the last changed time.
			entry_entry["LastChangedTime"] = entry._LastChangedTime;

			//Add the entry entry.
			entries_entry.emplace_back(entry_entry);
		}

		//Write the JSON!
		{
			//Open the file.
			std::ofstream file{ file_path };

			//Write the JSON.
			file << std::setw(4) << json;

			//Close the file.
			file.close();
		}
	}

private:

	//Enumeration covering all versions.
	enum class Version : uint64
	{
		BASE,

		CURRENT_VERSION
	};

	//The entries.
	DynamicArray<Entry> _Entries;

};