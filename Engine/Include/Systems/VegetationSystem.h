#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Vegetation.
#include <Vegetation/VegetationAddTypeInformation.h>
#include <Vegetation/VegetationTypeInformation.h>
#include <Vegetation/VegetationTypeInformationUpdate.h>

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
	*	Updates the vegetation system synchronously during the closing update phase.
	*/
	void OpeningUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the vegetation type informations.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<VegetationTypeInformation> *const RESTRICT GetVegetationTypeInformations() const NOEXCEPT
	{
		return &_VegetationTypeInformations;
	}

	/*
	*	Adds a vegetation type.
	*/
	void AddVegetationType(const VegetationAddTypeInformation &addInformation) NOEXCEPT;

private:

	//The vegetation type informations.
	DynamicArray<VegetationTypeInformation> _VegetationTypeInformations;

	//The update task.
	Task _UpdateTask;

	//The current camera position.
	Vector3 _CurrentCameraPosition;

	DynamicArray<VegetationTypeInformationUpdate> _VegetationTypeInformationUpdates;

	/*
	*	Updates the vegetation type informations.
	*/
	void UpdateVegetationTypeInformations() NOEXCEPT;

	/*
	*	Invalidates one patch.
	*/
	void InvalidatePatch(VegetationTypeInformation *const RESTRICT information, const uint8 index) NOEXCEPT;

	/*
	*	Updates the vegetation system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Updates a single vegetation type information.
	*/
	void UpdateVegetationTypeInformation(VegetationTypeInformation *const RESTRICT information) NOEXCEPT;

	/*
	*	Generates the transformations.
	*/
	void GenerateTransformations(const GridPoint &gridPoint, const VegetationTypeProperties &properties, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT;

};