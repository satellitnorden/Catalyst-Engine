#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Spinlock.h>
#include <Multithreading/Task.h>

/*
*	Declares an update system phase.
*/
#define DECLARE_UPDATE_SYSTEM_PHASE(PHASE)															\
public:																								\
	void Pre ## PHASE ## SystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;	\
	void Post ## PHASE ## SystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;	\
	void RegisterSynchronous ## PHASE(Updateable *const RESTRICT newUpdate);						\
	void RegisterAsynchronous ## PHASE(Updateable *const RESTRICT newUpdate);						\
private:																							\
	DynamicArray<Updateable *RESTRICT> _Synchronous ## PHASE ## s;									\
	DynamicArray<AsynchronousUpdateData> _Asynchronous ## PHASE ## s;

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

	//Denotes whether or not this asynchronous update should be de-registered.
	bool _ShouldDeRegister{ false };

};

class UpdateSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(UpdateSystem);

	/*
	*	Default constructor.
	*/
	UpdateSystem() NOEXCEPT { }

	//Declare the update system phases.
	DECLARE_UPDATE_SYSTEM_PHASE(LogicUpdate);

};