//Header file.
#include <Systems/UpdateSystem.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(UpdateSystem);

/*
*	Pre-updates the update system synchronously.
*/
void UpdateSystem::PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Kick off all asynchronous pre-updates.
	for (AsynchronousUpdateData *const RESTRICT data : _AsynchronousPreUpdates)
	{
		data->_Context = context;

		TaskSystem::Instance->ExecuteTask(&data->_Task);
	}

	//Execute the synchronous pre-updates.
	for (uint64 i = 0; i < _SynchronousPreUpdates.Size();)
	{
		if (!_SynchronousPreUpdates[i]->PreUpdateSynchronous(context))
		{
			_SynchronousPreUpdates.EraseAt(i);
		}

		else
		{
			++i;
		}
	}

	//Wait for all asynchronous pre-updates to finish.
	for (AsynchronousUpdateData *const RESTRICT data : _AsynchronousPreUpdates)
	{
		data->_Task.WaitFor();
	}

	//De-register all asynchronous pre-updates that should be de-registered
	for (uint64 i = 0; i < _AsynchronousPreUpdates.Size();)
	{
		if (_AsynchronousPreUpdates[i]->_ShouldDeRegister)
		{
			AsynchronousUpdateData *const RESTRICT data{ _AsynchronousPreUpdates[i] };
			_AsynchronousPreUpdates.EraseAt(i);
			delete data;
		}

		else
		{
			++i;
		}
	}
}

/*
*	Updates the update system synchronously.
*/
void UpdateSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Post-updates the update system synchronously.
*/
void UpdateSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Registers a synchronous pre-update.
*/
void UpdateSystem::RegisterSynchronousPreUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the synchronous pre-updates.
	_SynchronousPreUpdates.EmplaceSlow(newUpdate);
}

/*
*	Registers an asynchronous pre-update.
*/
void UpdateSystem::RegisterAsynchronousPreUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the asynchronous pre-updates.
	_AsynchronousPreUpdates.EmplaceSlow(new AsynchronousUpdateData);

	//Set up the asynchronous update data.
	_AsynchronousPreUpdates.Back()->_Task._Function = [](void *const RESTRICT arguments)
	{
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };

		if (!data->_UpdateAble->PreUpdateAsynchronous(data->_Context))
		{
			data->_ShouldDeRegister = true;
		}
	};
	_AsynchronousPreUpdates.Back()->_Task._Arguments = _AsynchronousPreUpdates.Back();
	_AsynchronousPreUpdates.Back()->_UpdateAble = newUpdate;
}