#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>

//Resources
#include <Resources/Core/ResourcesCore.h>

namespace SoundBankCreator
{

	class SoundBankCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The sound bank file path.
		const char *RESTRICT _SoundBank;

	};


	/*
	*	Creates a sound bank file.
	*/
	static void CreateSoundBank(const SoundBankCreationParameters &parameters) noexcept
	{
		//What should the collection be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> outputFile{ fileName.CString() };

		//Write the resource type to the output file.
		constexpr ResourceType resourceType{ ResourceType::SoundBank };
		outputFile.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the output file.
		const HashString resourceID{ parameters._ID };
		outputFile.Write(&resourceID, sizeof(HashString));

		//Open the sound bank file.
		BinaryFile<IOMode::In> soundBankFile{ parameters._SoundBank };

		//Get the size of the sound bank file.
		const uint64 soundBankFileSize{ soundBankFile.Size() };

		//Write the size of the sound bank file to the output file.
		outputFile.Write(&soundBankFileSize, sizeof(uint64));

		//Read the data in the sound bank file.
		void *const RESTRICT soundBankFileData{ MemoryUtilities::AllocateMemory(soundBankFileSize) };
		soundBankFile.Read(soundBankFileData, soundBankFileSize);

		//Write the sound bank file data to the output file.
		outputFile.Write(soundBankFileData, soundBankFileSize);

		//Free the sound bank file data.
		MemoryUtilities::FreeMemory(soundBankFileData);

		//Close the sound bank file.
		soundBankFile.Close();

		//Close the output file.
		outputFile.Close();
	}

}