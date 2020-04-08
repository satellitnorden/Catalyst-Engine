//Header file.
#include <Systems/SaveSystem.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>

//Systems.
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
}

/*
*	Updates the save system during the sequential update phase.
*/
void SaveSystem::SequentialUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Check if any saves have been requested, and if the task is not executing. If so, fire it off!
	if ((_RequestedLoadsMask != 0 || _RequestedSavesMask != 0) && _ProcessSavesTask.IsExecuted())
	{
		_ProcessLoadsMask = _RequestedLoadsMask;
		_RequestedLoadsMask = 0;

		_ProcessSavesMask = _RequestedSavesMask;
		_RequestedSavesMask = 0;

		TaskSystem::Instance->ExecuteTask(&_ProcessSavesTask);
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

	//Request a load for the new entry.
	RequestLoad(entry._SaveMask);
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
			//Does the file exist?
			if (!File::Exists(entry._File.Data()))
			{			
				//Determine the size required for the save.
				const uint64 size{ entry._SaveSizeCallback() };

				//Allocate the memory required for the save.
				void *const RESTRICT save_data{ Memory::Allocate(size) };

				//Set the default values.
				entry._DefaultValuesCallback(save_data);

				//Create the file.
				File::Create(entry._File.Data());

				//Write it to file.
				BinaryFile<IOMode::Out> file{ entry._File.Data() };
				file.Write(save_data, size);
				file.Close();

				//Call the load callback.
				entry._LoadCallback(size, save_data);

				//Free the memory.
				Memory::Free(save_data);
			}

			else
			{
				//Open the file.
				BinaryFile<IOMode::In> file{ entry._File.Data() };

				//Allocate the memory required for the save.
				void* const RESTRICT save_data{ Memory::Allocate(file.Size()) };

				//Read the saved data.
				file.Read(save_data, file.Size());
				file.Close();

				//Call the load callback.
				entry._LoadCallback(file.Size(), save_data);

				//Free the memory.
				Memory::Free(save_data);
			}
		}

		//Does this save entry need to be saved?
		if (entry._SaveMask & _ProcessSavesMask)
		{
			//Determine the size required for the save.
			const uint64 size{ entry._SaveSizeCallback() };

			//Allocate the memory required for the save.
			void *const RESTRICT save_data{ Memory::Allocate(size) };

			//Call the save callback.
			entry._SaveCallback(save_data);

			//Write it to file.
			BinaryFile<IOMode::Out> file{ entry._File.Data() };
			file.Write(save_data, size);
			file.Close();

			//Free the memory.
			Memory::Free(save_data);
		}
	}
}