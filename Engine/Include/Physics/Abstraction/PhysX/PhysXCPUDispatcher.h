#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxCore.h>

class PhysXCPUDispatcher final : public physx::PxCpuDispatcher
{

public:

	/*
	*	The submit task function.
	*/
	void submitTask(physx::PxBaseTask& task) override
	{
		//Just run the task, for now. (:
		task.run();
		task.release();
	}

	/*
	*	The get worker count function.
	*/
	uint32_t getWorkerCount() const override
	{
		return 0;
	}

};