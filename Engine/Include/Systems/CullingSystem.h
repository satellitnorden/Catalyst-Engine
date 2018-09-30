#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

class CullingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CullingSystem);

	/*
	*	Default constructor.
	*/
	CullingSystem() NOEXCEPT { }

	/*
	*	Initializes the culling system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the culling sustem during the culling update phase.
	*/
	void CullingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Waits for the vegetation culling to finish.
	*/
	void WaitForVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::Vegetation)].WaitFor();
	}

private:

	//Enumeration covering all culling tasks.
	enum class CullingTask : uint8
	{
		Vegetation,

		NumberOfCullingTasks
	};

	//Container for all culling tasks.
	StaticArray<Task, UNDERLYING(CullingTask::NumberOfCullingTasks)> _CullingTasks;

	/*
	*	Culls vegetation.
	*/
	void CullVegetation() NOEXCEPT;

};