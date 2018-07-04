#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

class SoundBankCreator final
{

public:

	/*
	*	Creates a sound bank resource file.
	*/
	static void CreateSoundBank(const char *const RESTRICT arguments[]) noexcept
	{
		//What should the file be called?
		DynamicString fileName{ arguments[0] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::SoundBank };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[1] };
		file.Write(&resourceID, sizeof(HashString));

		//Open the sound bank file.
		BinaryFile<IOMode::In> soundBankFile{ arguments[2] };

		//Get the size of the sound bank file.
		const uint64 soundBankFileSize{ soundBankFile.Size() };

		//Write the size of the sound bank.
		file.Write(&soundBankFileSize, sizeof(uint64));

		//Read the data in the sound bank file.
		void *const RESTRICT soundBankFileData{ MemoryUtilities::AllocateMemory(soundBankFileSize) };
		soundBankFile.Read(soundBankFileData, soundBankFileSize);

		//Write the sound bank file data to the file.
		file.Write(soundBankFileData, soundBankFileSize);

		//Free the sound bank file data.
		MemoryUtilities::FreeMemory(soundBankFileData);

		//Close the sound bank file.
		soundBankFile.Close();

		//Close the file.
		file.Close();
	}

};