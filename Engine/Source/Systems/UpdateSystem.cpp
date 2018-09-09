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
	//Kick off all asynchronous pre-updates.
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdates)
	{
		data._Context = context;

		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Execute the synchronous pre-updates.
	for (uint64 i = 0; i < _SynchronousPreUpdates.Size(); ++i)
	{
		if (!_SynchronousPreUpdates[i]->PreUpdateSynchronous(context))
		{
			DeRegisterSynchronousPreUpdate(_SynchronousPreUpdates[i]);
		}
	}

	//Wait for all asynchronous pre-updates to finish.
	for (AsynchronousUpdateData &data : _AsynchronousPreUpdates)
	{
		data._Task.WaitFor();
	}
}

/*
*	Updates the update system synchronously.
*/
void UpdateSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute the synchronous updates.
	for (uint64 i = 0; i < _SynchronousUpdates.Size(); ++i)
	{
		if (!_SynchronousUpdates[i]->UpdateSynchronous(context))
		{
			DeRegisterSynchronousUpdate(_SynchronousUpdates[i]);
		}
	}
}

/*
*	Post-updates the update system synchronously.
*/
void UpdateSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute the synchronous post-updates.
	for (uint64 i = 0; i < _SynchronousPostUpdates.Size(); ++i)
	{
		if (!_SynchronousPostUpdates[i]->PostUpdateSynchronous(context))
		{
			DeRegisterSynchronousPostUpdate(_SynchronousPostUpdates[i]);
		}
	}
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
*	De-registers a synchronous pre-update.
*/
void UpdateSystem::DeRegisterSynchronousPreUpdate(Updateable *const RESTRICT update)
{
	//Remove the update from the synchronous pre-updates.
	_SynchronousPreUpdates.Erase(update);
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
*	De-registers a synchronous update.
*/
void UpdateSystem::DeRegisterSynchronousUpdate(Updateable *const RESTRICT update)
{
	//Remove the update from the synchronous updates.
	_SynchronousUpdates.Erase(update);
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
*	De-registers a synchronous post-update.
*/
void UpdateSystem::DeRegisterSynchronousPostUpdate(Updateable *const RESTRICT update)
{
	//Remove the update from the synchronous post-updates.
	_SynchronousPostUpdates.Erase(update);
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
			UpdateSystem::Instance->DeRegisterAsynchronousPreUpdate(data->_UpdateAble);
		}
	};
	_AsynchronousPreUpdates.Back()._Task._Arguments = &_AsynchronousPreUpdates.Back();
	_AsynchronousPreUpdates.Back()._UpdateAble = newUpdate;
}

/*
*	De-registers an asynchronous pre-update.
*/
void UpdateSystem::DeRegisterAsynchronousPreUpdate(Updateable *const RESTRICT update)
{
	//Remove the update from the asynchronous pre-updates.
	for (uint64 i = 0, size = _AsynchronousPreUpdates.Size(); i < size; ++i)
	{
		if (_AsynchronousPreUpdates[i]._UpdateAble == update)
		{
			_AsynchronousPreUpdates.EraseAt(i);

			return;
		}
	}
}