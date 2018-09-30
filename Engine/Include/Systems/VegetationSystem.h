#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Vegetation.
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
	*	Returns the vegetation type informations, const.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<VegetationTypeInformation> *const RESTRICT GetVegetationTypeInformations() const NOEXCEPT
	{
		return &_VegetationTypeInformations;
	}

	/*
	*	Returns the vegetation type informations, non-const.
	*/
	RESTRICTED NO_DISCARD DynamicArray<VegetationTypeInformation> *const RESTRICT GetVegetationTypeInformations() NOEXCEPT
	{
		return &_VegetationTypeInformations;
	}

	/*
	*	Adds a vegetation type.
	*/
	void AddVegetationType(const VegetationTypeProperties &properties, const VegetationModel &model, const VegetationMaterial &material) NOEXCEPT;

private:

	//The vegetation type informations.
	DynamicArray<VegetationTypeInformation> _VegetationTypeInformations;

	//The update task.
	Task _UpdateTask;

	//The current camera position.
	Vector3 _CurrentCameraPosition;
	
	//The vegetation type information update.
	VegetationTypeInformationUpdate _VegetationTypeInformationUpdate;

	/*
	*	Processes the vegetation type information update.
	*/
	void ProcessVegetationTypeInformationUpdate() NOEXCEPT;

	/*
	*	Invalidates one patch.
	*/
	void InvalidatePatch(VegetationTypeInformation *const RESTRICT information, const uint8 index) NOEXCEPT;

	/*
	*	Updates the vegetation system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Generates the transformations.
	*/
	void GenerateTransformations(const GridPoint &gridPoint, const VegetationTypeProperties &properties, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT;

};