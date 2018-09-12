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
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdates)
	{
		data._Context = context;
		data._Task._Arguments = &data;

		TaskSystem::Instance->ExecuteTask(&data._Task);
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
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdates)
	{
		data._Task.WaitFor();
	}

	//De-register all asynchronous pre-updates that should be de-registered.
	for (uint64 i = 0; i < _AsynchronousPreUpdates.Size();)
	{
		if (_AsynchronousPreUpdates[i]._ShouldDeRegister)
		{
			_AsynchronousPreUpdates.EraseAt(i);
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
	//Kick off all asynchronous updates.
	for (AsynchronousUpdateData &data : _AsynchronousUpdates)
	{
		data._Context = context;
		data._Task._Arguments = &data;

		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute the synchronous updates.
	for (uint64 i = 0; i < _SynchronousUpdates.Size();)
	{
		if (!_SynchronousUpdates[i]->PreUpdateSynchronous(context))
		{
			_SynchronousUpdates.EraseAt(i);
		}

		else
		{
			++i;
		}
	}

	//Wait for all asynchronous updates to finish.
	for (AsynchronousUpdateData &data : _AsynchronousUpdates)
	{
		data._Task.WaitFor();
	}

	//De-register all asynchronous updates that should be de-registered.
	for (uint64 i = 0; i < _AsynchronousUpdates.Size();)
	{
		if (_AsynchronousUpdates[i]._ShouldDeRegister)
		{
			_AsynchronousUpdates.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Post-updates the update system synchronously.
*/
void UpdateSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Kick off all asynchronous post-updates.
	for (AsynchronousUpdateData &data : _AsynchronousPostUpdates)
	{
		data._Context = context;
		data._Task._Arguments = &data;

		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute the synchronous post-updates.
	for (uint64 i = 0; i < _SynchronousPostUpdates.Size();)
	{
		if (!_SynchronousPostUpdates[i]->PostUpdateSynchronous(context))
		{
			_SynchronousPostUpdates.EraseAt(i);
		}

		else
		{
			++i;
		}
	}

	//Wait for all asynchronous post-updates to finish.
	for (AsynchronousUpdateData &data : _AsynchronousPostUpdates)
	{
		data._Task.WaitFor();
	}

	//De-register all asynchronous post-updates that should be de-registered.
	for (uint64 i = 0; i < _AsynchronousPostUpdates.Size();)
	{
		if (_AsynchronousPostUpdates[i]._ShouldDeRegister)
		{
			_AsynchronousPostUpdates.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Releases the update system.
*/
void UpdateSystem::ReleaseSystem() NOEXCEPT
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
	_AsynchronousPreUpdates.EmplaceSlow();

	//Set up the asynchronous update data.
	_AsynchronousPreUpdates.Back()._Task._Function = [](void *const RESTRICT arguments)
	{
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };

		if (!data->_UpdateAble->PreUpdateAsynchronous(data->_Context))
		{
			data->_ShouldDeRegister = true;
		}
	};
	_AsynchronousPreUpdates.Back()._UpdateAble = newUpdate;
}

/*
*	Registers a synchronous update.
*/
void UpdateSystem::RegisterSynchronousUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the synchronous updates.
	_SynchronousUpdates.EmplaceSlow(newUpdate);
}

/*
*	Registers an asynchronous update.
*/
void UpdateSystem::RegisterAsynchronousUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the asynchronous updates.
	_AsynchronousUpdates.EmplaceSlow();

	//Set up the asynchronous update data.
	_AsynchronousUpdates.Back()._Task._Function = [](void *const RESTRICT arguments)
	{
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };

		if (!data->_UpdateAble->UpdateAsynchronous(data->_Context))
		{
			data->_ShouldDeRegister = true;
		}
	};
	_AsynchronousUpdates.Back()._UpdateAble = newUpdate;
}

/*
*	Registers a synchronous post-update.
*/
void UpdateSystem::RegisterSynchronousPostUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the synchronous post-updates.
	_SynchronousPostUpdates.EmplaceSlow(newUpdate);
}

/*
*	Registers an asynchronous post-update.
*/
void UpdateSystem::RegisterAsynchronousPostUpdate(Updateable *const RESTRICT newUpdate)
{
	//Add the update to the asynchronous post-updates.
	_AsynchronousPostUpdates.EmplaceSlow();

	//Set up the asynchronous update data.
	_AsynchronousPostUpdates.Back()._Task._Function = [](void *const RESTRICT arguments)
	{
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };

		if (!data->_UpdateAble->PostUpdateAsynchronous(data->_Context))
		{
			data->_ShouldDeRegister = true;
		}
	};
	_AsynchronousPostUpdates.Back()._UpdateAble = newUpdate;
}