//Header file.
#include <Systems/SaveSystem.h>

//File handling.
#include <FileHandling/BinaryFile.h>
#include <FileHandling/FileHandlingUtilities.h>

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
*	Registers a save entry.
*/
void SaveSystem::RegisterSaveEntry(const SaveEntry &entry) NOEXCEPT
{
	//Add the save entry.
	_SaveEntries.EmplaceSlow(entry);

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
			//First of all, does the file exist?
			if (!FileHandlingUtilities::DoesFileExist(entry._File.Data()))
			{
				//If not, create it.
				FileHandlingUtilities::CreateFile(entry._File.Data());

				//Allocate the memory required for the save.
				void *const RESTRICT save_data{ Memory::Allocate(entry._Size) };

				//Set the default values.
				entry._DefaultValuesCallback(save_data);

				//Write it to file.
				BinaryFile<IOMode::Out> file{ entry._File.Data() };
				file.Write(save_data, entry._Size);
				file.Close();

				//Call the load callback.
				entry._LoadCallback(save_data);

				//Free the memory.
				Memory::Free(save_data);
			}

			else
			{
				//Allocate the memory required for the save.
				void *const RESTRICT save_data{ Memory::Allocate(entry._Size) };

				//Read it from file.
				BinaryFile<IOMode::In> file{ entry._File.Data() };
				file.Read(save_data, entry._Size);
				file.Close();

				//Call the load callback.
				entry._LoadCallback(save_data);

				//Free the memory.
				Memory::Free(save_data);
			}
		}

		//Does this save entry need to be saved?
		if (entry._SaveMask & _ProcessSavesMask)
		{
			//Allocate the memory required for the save.
			void *const RESTRICT save_data{ Memory::Allocate(entry._Size) };

			//Call the save callback.
			entry._SaveCallback(save_data);

			//Write it to file.
			BinaryFile<IOMode::Out> file{ entry._File.Data() };
			file.Write(save_data, entry._Size);
			file.Close();

			//Free the memory.
			Memory::Free(save_data);
		}
	}
}