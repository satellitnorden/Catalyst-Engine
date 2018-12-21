//Header file.
#include <Systems/UpdateSystem.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(UpdateSystem);

/*
*	Defines an update system phase.
*/
#define DEFINE_UPDATE_SYSTEM_PHASE(PHASE)																				\
void UpdateSystem::RegisterSynchronous ## PHASE(Updateable *const RESTRICT newUpdate)									\
{																														\
	_Synchronous ## PHASE ## s.EmplaceSlow(newUpdate);																	\
}																														\
																														\
void UpdateSystem::RegisterAsynchronous ## PHASE(Updateable *const RESTRICT newUpdate)									\
{																														\
	_Asynchronous ## PHASE ## s.EmplaceSlow();																			\
	_Asynchronous ## PHASE ## s.Back()._Task._Function = [](void *const RESTRICT arguments)								\
	{																													\
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };	\
																														\
		if (!data->_UpdateAble->PHASE ## Asynchronous(data->_Context))													\
		{																												\
			data->_ShouldDeRegister = true;																				\
		}																												\
	};																													\
	_Asynchronous ## PHASE ## s.Back()._UpdateAble = newUpdate;															\
}

//Define the update system phases.
DEFINE_UPDATE_SYSTEM_PHASE(LogicUpdate);

/*
*	Updates the update system synchronously.
*/
void UpdateSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	for (AsynchronousUpdateData &data : _AsynchronousLogicUpdates)
	{
		data._Context = context;
		data._Task._Arguments = &data;
		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	for (uint64 i = 0; i < _SynchronousLogicUpdates.Size();)
	{
		if (!_SynchronousLogicUpdates[i]->LogicUpdateSynchronous(context))
		{
			_SynchronousLogicUpdates.EraseAt(i);
		}
		
		else
		{
			++i;
		}
	}
	
	for (AsynchronousUpdateData &data : _AsynchronousLogicUpdates)
	{
		data._Task.WaitFor();
	}

	for (uint64 i = 0; i < _AsynchronousLogicUpdates.Size();)
	{
		if (_AsynchronousLogicUpdates[i]._ShouldDeRegister)
		{
			_AsynchronousLogicUpdates.EraseAt(i);
		}
		
		else
		{
			++i;
		}
	}
}