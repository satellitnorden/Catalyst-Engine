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
void UpdateSystem::Pre ## PHASE ## SystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT				\
{																														\
	for (AsynchronousUpdateData &data : _Asynchronous ## PHASE ## s)													\
	{																													\
		data._Context = context;																						\
		data._Task._Arguments = &data;																					\
		TaskSystem::Instance->ExecuteTask(&data._Task);																	\
	}																													\
}																														\
																														\
void UpdateSystem::Post ## PHASE ## SystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT				\
{																														\
	for (uint64 i = 0; i < _Synchronous ## PHASE ## s.Size();)															\
	{																													\
		if (!_Synchronous ## PHASE ## s[i]-> ## PHASE ## Synchronous(context))											\
		{																												\
			_Synchronous ## PHASE ## s.EraseAt(i);																		\
		}																												\
																														\
		else																											\
		{																												\
			++i;																										\
		}																												\
	}																													\
																														\
	for (AsynchronousUpdateData &data : _Asynchronous ## PHASE ## s)													\
	{																													\
		data._Task.WaitFor();																							\
	}																													\
																														\
	for (uint64 i = 0; i < _Asynchronous ## PHASE ## s.Size();)															\
	{																													\
		if (_Asynchronous ## PHASE ## s[i]._ShouldDeRegister)															\
		{																												\
			_Asynchronous ## PHASE ## s.EraseAt(i);																		\
		}																												\
																														\
		else																											\
		{																												\
			++i;																										\
		}																												\
	}																													\
}																														\
																														\
void UpdateSystem::RegisterSynchronous ## PHASE ## (Updateable *const RESTRICT newUpdate)								\
{																														\
	_Synchronous ## PHASE ## s.EmplaceSlow(newUpdate);																	\
}																														\
																														\
void UpdateSystem::RegisterAsynchronous ## PHASE ## (Updateable *const RESTRICT newUpdate)								\
{																														\
	_Asynchronous ## PHASE ## s.EmplaceSlow();																			\
	_Asynchronous ## PHASE ## s.Back()._Task._Function = [](void *const RESTRICT arguments)								\
	{																													\
		AsynchronousUpdateData *const RESTRICT data{ static_cast<AsynchronousUpdateData *const RESTRICT>(arguments) };	\
																														\
		if (!data->_UpdateAble-> ## PHASE ## Asynchronous(data->_Context))												\
		{																												\
			data->_ShouldDeRegister = true;																				\
		}																												\
	};																													\
	_Asynchronous ## PHASE ## s.Back()._UpdateAble = newUpdate;															\
}

//Define the update system phases.
DEFINE_UPDATE_SYSTEM_PHASE(OpeningUpdate);
DEFINE_UPDATE_SYSTEM_PHASE(LogicUpdate);
DEFINE_UPDATE_SYSTEM_PHASE(RenderUpdate);
DEFINE_UPDATE_SYSTEM_PHASE(ClosingUpdate);