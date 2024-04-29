#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Task.h>

//Save.
#include <Save/SaveCore.h>
#include <Save/SaveEntry.h>

//Systems.
#include <Systems/System.h>

class SaveSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		SaveSystem
	);

	/*
	*	Default constructor.
	*/
	SaveSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the save system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the save system sequentially.
	*/
	void SequentialUpdate() NOEXCEPT;

	/*
	*	Terminates the save system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Registers a save entry.
	*/
	void RegisterSaveEntry(const SaveEntry &entry) NOEXCEPT;

	/*
	*	Requests a load.
	*/
	void RequestLoad(const uint64 save_mask) NOEXCEPT;

	/*
	*	Requests a save.
	*/
	void RequestSave(const uint64 save_mask) NOEXCEPT;

private:

	//The save entries.
	DynamicArray<SaveEntry> _SaveEntries;

	//Mask for the requested load.
	uint64 _RequestedLoadsMask{ 0 };

	//Mask for the loads to process.
	uint64 _ProcessLoadsMask{ 0 };

	//Mask for the requested saves.
	uint64 _RequestedSavesMask{ 0 };

	//Mask for the saves to process.
	uint64 _ProcessSavesMask{ 0 };

	//The process saves task.
	Task _ProcessSavesTask;

	/*
	*	Processes the saves.
	*/
	void ProcessSaves() NOEXCEPT;

	/*
	*	Loads a single entry.
	*/
	void LoadSingleEntry(const SaveEntry &entry) NOEXCEPT;

	/*
	*	Saves a single entry.
	*/
	void SaveSingleEntry(const SaveEntry &entry) NOEXCEPT;

};