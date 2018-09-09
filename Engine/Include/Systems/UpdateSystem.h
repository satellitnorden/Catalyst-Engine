#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

class UpdateSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(UpdateSystem);

	/*
	*	Default constructor.
	*/
	UpdateSystem() NOEXCEPT { }

	/*
	*	Pre-updates the update system synchronously.
	*/
	void PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the update system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Post-updates the update system synchronously.
	*/
	void PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers a synchronous pre-update.
	*/
	void RegisterSynchronousPreUpdate(Updateable *const RESTRICT newUpdate);

	/*
	*	De-registers a synchronous pre-update.
	*/
	void DeRegisterSynchronousPreUpdate(Updateable *const RESTRICT update);

	/*
	*	Registers a synchronous update.
	*/
	void RegisterSynchronousUpdate(Updateable *const RESTRICT newUpdate);

	/*
	*	De-registers a synchronous update.
	*/
	void DeRegisterSynchronousUpdate(Updateable *const RESTRICT update);

	/*
	*	Registers a synchronous post-update.
	*/
	void RegisterSynchronousPostUpdate(Updateable *const RESTRICT newUpdate);

	/*
	*	De-registers a synchronous post-update.
	*/
	void DeRegisterSynchronousPostUpdate(Updateable *const RESTRICT update);

	/*
	*	Registers an asynchronous pre-update.
	*/
	void RegisterAsynchronousPreUpdate(Updateable *const RESTRICT newUpdate);

	/*
	*	De-registers an asynchronous pre-update.
	*/
	void DeRegisterAsynchronousPreUpdate(Updateable *const RESTRICT update);

private:

	/*
	*	Asynchronous update data definition.
	*/
	class AsynchronousUpdateData final
	{
		
	public:

		//The task.
		Task _Task;

		//The update context.
		const UpdateContext *RESTRICT _Context;

		//The updateable.
		Updateable *RESTRICT _UpdateAble;

	};

	//Container for all synchronous pre-updates.
	DynamicArray<Updateable *RESTRICT> _SynchronousPreUpdates;

	//Container for all synchronous updates.
	DynamicArray<Updateable *RESTRICT> _SynchronousUpdates;

	//Container for all synchronous post-updates.
	DynamicArray<Updateable *RESTRICT> _SynchronousPostUpdates;

	//Container for all asynchronous pre-updates.
	DynamicArray<AsynchronousUpdateData> _AsynchronousPreUpdates;

};