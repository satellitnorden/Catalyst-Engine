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
	*	Pre-updates the culling system synchronously.
	*/
	void PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;


private:

	//Enumeration covering all culling tasks.
	enum class CullingTask : uint8
	{
		UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes,

		NumberOfCullingTasks
	};

	//Container for all culling tasks.
	StaticArray<Task, INDEX(CullingTask::NumberOfCullingTasks)> _CullingTasks;

	/*
	*	Initializes the culling tasks.
	*/
	void InitializeCullingTasks() NOEXCEPT;

	/*
	*	Update the world space axis-aligned bounding boxes of dynamic physical entities.
	*/
	void UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes() const NOEXCEPT;

};