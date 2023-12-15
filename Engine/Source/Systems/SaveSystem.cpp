//Header file.
#include <Systems/SaveSystem.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>

//Save.
#include <Save/SaveHeader.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SaveSystem);

/*
*	Initializes the save system.
*/
void SaveSystem::Initialize() NOEXCEPT
{
	//Initialize the process saves task.
	_ProcessSavesTask._Function = [](void* const RESTRICT)
	{
		SaveSystem::Instance->ProcessSaves();
	};
	_ProcessSavesTask._Arguments = nullptr;
	_ProcessSavesTask._ExecutableOnSameThread = false;

	//Register the update.
	CatalystEngineSystem::Instance->RegisterSequentialUpdate
	(
		[](void* const RESTRICT arguments)
		{
			static_cast<SaveSystem *const RESTRICT>(arguments)->SequentialUpdate();
		},
		this
	);
}

/*
*	Updates the save system sequentially.
*/
void SaveSystem::SequentialUpdate() NOEXCEPT
{
	//Check if any saves have been requested, and if the task is not executing. If so, fire it off!
	if ((_RequestedLoadsMask != 0 || _RequestedSavesMask != 0) && _ProcessSavesTask.IsExecuted())
	{
		_ProcessLoadsMask = _RequestedLoadsMask;
		_RequestedLoadsMask = 0;

		_ProcessSavesMask = _RequestedSavesMask;
		_RequestedSavesMask = 0;

		TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &_ProcessSavesTask);
	}
}

/*
*	Pre-terminates the save system.
*/
void SaveSystem::PreTerminate() NOEXCEPT
{
	//Save all save entries.
	_ProcessLoadsMask = 0;
	_ProcessSavesMask = UINT64_MAXIMUM;

	ProcessSaves();
}

/*
*	Registers a save entry.
*/
void SaveSystem::RegisterSaveEntry(const SaveEntry &entry) NOEXCEPT
{
	//Add the save entry.
	_SaveEntries.Emplace(entry);

	//Load the entry immediately, so that all initial data gets loaded when the game starts.
	LoadSingleEntry(entry);
}

/*
*	Requests a load.
*/
void SaveSystem::RequestLoad(const uint64 save_mask) NOEXCEPT
{
	//Add the requested load mask.
	_RequestedLoadsMask |= save_mask;
}

/*
*	Requests a save.
*/
void SaveSystem::RequestSave(const uint64 save_mask) NOEXCEPT
{
	//Add the requested save mask.
	_RequestedSavesMask |= save_mask;
}

/*
*	Processes the saves.
*/
void SaveSystem::ProcessSaves() NOEXCEPT
{
	//Go over all save entries, checking if they need to be saved or loaded.
	for (const SaveEntry &entry : _SaveEntries)
	{
		//Does this save entry need to be loaded?
		if (entry._SaveMask & _ProcessLoadsMask)
		{
			LoadSingleEntry(entry);
		}

		//Does this save entry need to be saved?
		if (entry._SaveMask & _ProcessSavesMask)
		{
			SaveSingleEntry(entry);
		}
	}
}



/*
*	Loads a single entry.
*/
void SaveSystem::LoadSingleEntry(const SaveEntry &entry) NOEXCEPT
{
	//Does the file exist?
	if (!File::Exists(entry._FilePath.Data()))
	{			
		//Construct the save header.
		SaveHeader save_header;

		save_header._Version = entry._CurrentVersionCallback();

		//Determine the size required for the save.
		const uint64 size{ entry._SaveSizeCallback() };

		//Allocate the memory required for the save.
		void *const RESTRICT save_data{ Memory::Allocate(size) };

		//Set the default values.
		entry._DefaultValuesCallback(save_data);

		//Create the file.
		File::CreateFile(entry._FilePath.Data());

		//Write it to file.
		BinaryFile<BinaryFileMode::OUT> file{ entry._FilePath.Data() };
		file.Write(&save_header, sizeof(SaveHeader));
		file.Write(save_data, size);
		file.Close();

		//Call the load callback.
		entry._LoadCallback(save_header._Version, save_data);

		//Free the memory.
		Memory::Free(save_data);
	}

	else
	{
		//Open the file.
		BinaryFile<BinaryFileMode::IN> file{ entry._FilePath.Data() };

		//Allocate the memory required for the save.
		void* const RESTRICT save_data{ Memory::Allocate(file.Size()) };

		//Read the saved data.
		file.Read(save_data, file.Size());
		file.Close();

		//Call the load callback.
		entry._LoadCallback(static_cast<SaveHeader *const RESTRICT>(save_data)->_Version, AdvancePointer(save_data, sizeof(SaveHeader)));

		//Free the memory.
		Memory::Free(save_data);
	}
}

/*
*	Saves a single entry.
*/
void SaveSystem::SaveSingleEntry(const SaveEntry &entry) NOEXCEPT
{
	//Construct the save header.
	SaveHeader save_header;

	save_header._Version = entry._CurrentVersionCallback();

	//Determine the size required for the save.
	const uint64 size{ entry._SaveSizeCallback() };

	//Allocate the memory required for the save.
	void *const RESTRICT save_data{ Memory::Allocate(size) };

	//Call the save callback.
	entry._SaveCallback(save_data);

	//Write it to file.
	BinaryFile<BinaryFileMode::OUT> file{ entry._FilePath.Data() };
	file.Write(&save_header, sizeof(SaveHeader));
	file.Write(save_data, size);
	file.Close();

	//Free the memory.
	Memory::Free(save_data);
}