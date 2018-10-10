#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Vegetation.
#include <Vegetation/GrassVegetationTypeInformation.h>
#include <Vegetation/GrassVegetationTypeInformationUpdate.h>

class VegetationSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VegetationSystem);

	/*
	*	Default constructor.
	*/
	VegetationSystem() NOEXCEPT { }

	/*
	*	Initializes the vegetation system.
	*/
	void InitializeSystem();

	/*
	*	Updates the vegetation system sequentially.
	*/
	void SequentialUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Returns the grass vegetation type informations, const.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<GrassVegetationTypeInformation> *const RESTRICT GetGrassVegetationTypeInformations() const NOEXCEPT
	{
		return &_GrassVegetationTypeInformations;
	}

	/*
	*	Returns the grass vegetation type informations, non-const.
	*/
	RESTRICTED NO_DISCARD DynamicArray<GrassVegetationTypeInformation> *const RESTRICT GetGrassVegetationTypeInformations() NOEXCEPT
	{
		return &_GrassVegetationTypeInformations;
	}

	/*
	*	Adds a grass vegetation type.
	*/
	void AddGrassVegetationType(const GrassVegetationTypeProperties &properties, const GrassModel &model, const GrassMaterial &material) NOEXCEPT;

private:

	//The update task.
	Task _UpdateTask;

	//The current camera position.
	Vector3 _CurrentCameraPosition;

	//The grass vegetation type information update.
	GrassVegetationTypeInformationUpdate _VegetationTypeInformationUpdate;

	//The grass vegetation type informations.
	DynamicArray<GrassVegetationTypeInformation> _GrassVegetationTypeInformations;

	/*
	*	Processes the vegetation type information update.
	*/
	void ProcessVegetationTypeInformationUpdate() NOEXCEPT;

	/*
	*	Invalidates one patch.
	*/
	void InvalidatePatch(GrassVegetationTypeInformation *const RESTRICT information, const uint8 index) NOEXCEPT;

	/*
	*	Updates the vegetation system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Generates the transformations.
	*/
	void GenerateTransformations(const GridPoint2 &gridPoint, const GrassVegetationTypeProperties &properties, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT;

};