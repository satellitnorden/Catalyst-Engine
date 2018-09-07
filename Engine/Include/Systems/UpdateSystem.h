#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/General/UpdateFunctionContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

/*
*	Definition for an update function.
*	An update function receives an update function context containing this frame's update context as well as optional arguments.
*/
using UpdateFunction = void(*)(const UpdateFunctionContext *const RESTRICT context);

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
	*	Registers a synchronous pre-update function.
	*/
	void RegisterSynchronousPreUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

	/*
	*	Registers an asynchronous pre-update function.
	*/
	void RegisterAsynchronousPreUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

	/*
	*	Registers a synchronous update function.
	*/
	void RegisterSynchronousUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

	/*
	*	Registers an asynchronous update function.
	*/
	void RegisterAsynchronousUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

	/*
	*	Registers a synchronous post-update function.
	*/
	void RegisterSynchronousPostUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

	/*
	*	Registers an asynchronous post-update function.
	*/
	void RegisterAsynchronousPostUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments = nullptr);

private:

	/*
	*	Synchronous update data definition.
	*/
	class SynchronousUpdateData final
	{

	public:

		//The function.
		const UpdateFunction _Function;

		//The arguments.
		void *RESTRICT _Arguments;

		/*
		*	Constructor taking all values as arguments.
		*/
		SynchronousUpdateData(const UpdateFunction function, void *const RESTRICT arguments) NOEXCEPT
			:
			_Function(function),
			_Arguments(arguments)
		{

		}

	};

	/*
	*	Asynchronous update data definition.
	*/
	class AsynchronousUpdateData final
	{

	public:

		//The task.
		Task _Task;

		//The context.
		UpdateFunctionContext _Context;

	};

	//Container for all synchronous pre-update data.
	DynamicArray<SynchronousUpdateData> _SynchronousPreUpdateData;

	//Container for all asynchronous pre-update data.
	DynamicArray<AsynchronousUpdateData> _AsynchronousPreUpdateData;

	//Container for all synchronous update data.
	DynamicArray<SynchronousUpdateData> _SynchronousUpdateData;

	//Container for all asynchronous update data.
	DynamicArray<AsynchronousUpdateData> _AsynchronousUpdateData;

	//Container for all synchronous post-update data.
	DynamicArray<SynchronousUpdateData> _SynchronousPostUpdateData;

	//Container for all asynchronous post-update data.
	DynamicArray<AsynchronousUpdateData> _AsynchronousPostUpdateData;

};