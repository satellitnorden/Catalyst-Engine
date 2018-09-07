//Header file.
#include <Systems/UpdateSystem.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(UpdateSystem);

/*
*	Pre-updates the update system synchronously.
*/
void UpdateSystem::PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Kick off all asynchronous pre-update functions.
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdateData)
	{
		data._Context._Context = context;
		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute all synchronous pre-update functions.
	UpdateFunctionContext functionContext;
	functionContext._Context = context;

	for (const SynchronousUpdateData &data : _SynchronousPreUpdateData)
	{
		functionContext._Arguments = data._Arguments;
		data._Function(&functionContext);
	}

	//Wait for all asynchronous pre-update functions to finish.
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdateData)
	{
		data._Task.WaitFor();
	}
}

/*
*	Updates the update system synchronously.
*/
void UpdateSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Kick off all asynchronous update functions.
	for (AsynchronousUpdateData &data : _AsynchronousUpdateData)
	{
		data._Context._Context = context;
		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute all synchronous update functions.
	UpdateFunctionContext functionContext;
	functionContext._Context = context;

	for (const SynchronousUpdateData &data : _SynchronousUpdateData)
	{
		functionContext._Arguments = data._Arguments;
		data._Function(&functionContext);
	}

	//Wait for all asynchronous update functions to finish.
	for (AsynchronousUpdateData &data : _AsynchronousUpdateData)
	{
		data._Task.WaitFor();
	}
}

/*
*	Post-updates the update system synchronously.
*/
void UpdateSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Kick off all asynchronous post-update functions.
	for (AsynchronousUpdateData &data : _AsynchronousPostUpdateData)
	{
		data._Context._Context = context;
		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute all synchronous post-update functions.
	UpdateFunctionContext functionContext;
	functionContext._Context = context;

	for (const SynchronousUpdateData &data : _SynchronousPostUpdateData)
	{
		functionContext._Arguments = data._Arguments;
		data._Function(&functionContext);
	}

	//Wait for all asynchronous post-update functions to finish.
	for (AsynchronousUpdateData &data : _AsynchronousPostUpdateData)
	{
		data._Task.WaitFor();
	}
}

/*
*	Registers a synchronous pre-update function.
*/
void UpdateSystem::RegisterSynchronousPreUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the synchronous pre-update data.
	_SynchronousPreUpdateData.EmplaceSlow(function, arguments);
}

/*
*	Registers an asynchronous pre-update function.
*/
void UpdateSystem::RegisterAsynchronousPreUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the asynchronous pre-update data.
	_AsynchronousPreUpdateData.EmplaceSlow();

	AsynchronousUpdateData &data{ _AsynchronousPreUpdateData.Back() };
	data._Context._Arguments = arguments;
	data._Task._Function = reinterpret_cast<void(*)(void *const RESTRICT)>(function);
	data._Task._Arguments = &data._Context;
}

/*
*	Registers a synchronous update function.
*/
void UpdateSystem::RegisterSynchronousUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the synchronous update data.
	_SynchronousUpdateData.EmplaceSlow(function, arguments);
}

/*
*	Registers an asynchronous update function.
*/
void UpdateSystem::RegisterAsynchronousUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the asynchronous update data.
	_AsynchronousUpdateData.EmplaceSlow();

	AsynchronousUpdateData &data{ _AsynchronousUpdateData.Back() };
	data._Context._Arguments = arguments;
	data._Task._Function = reinterpret_cast<void(*)(void *const RESTRICT)>(function);
	data._Task._Arguments = &data._Context;
}

/*
*	Registers a synchronous post-update function.
*/
void UpdateSystem::RegisterSynchronousPostUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the synchronous post-update data.
	_SynchronousPostUpdateData.EmplaceSlow(function, arguments);
}

/*
*	Registers an asynchronous post-update function.
*/
void UpdateSystem::RegisterAsynchronousPostUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments)
{
	//Add the update function to the asynchronous post-update data.
	_AsynchronousPostUpdateData.EmplaceSlow();

	AsynchronousUpdateData &data{ _AsynchronousPostUpdateData.Back() };
	data._Context._Arguments = arguments;
	data._Task._Function = reinterpret_cast<void(*)(void *const RESTRICT)>(function);
	data._Task._Arguments = &data._Context;
}