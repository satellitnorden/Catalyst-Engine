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
#include <Vegetation/SolidVegetationTypeInformation.h>
#include <Vegetation/SolidVegetationTypeInformationUpdate.h>

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
	*	Adds a grass vegetation type.
	*/
	void AddGrassVegetationType(const GrassVegetationTypeProperties &properties, const StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> &models, const GrassVegetationMaterial &material) NOEXCEPT;

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
	*	Adds a solid vegetation type.
	*/
	void AddSolidVegetationType(const SolidVegetationTypeProperties &properties, const StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> &models, const PhysicalMaterial &material) NOEXCEPT;

	/*
	*	Returns the solid vegetation type informations, const.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<SolidVegetationTypeInformation> *const RESTRICT GetSolidVegetationTypeInformations() const NOEXCEPT
	{
		return &_SolidVegetationTypeInformations;
	}

	/*
	*	Returns the solid vegetation type informations, non-const.
	*/
	RESTRICTED NO_DISCARD DynamicArray<SolidVegetationTypeInformation> *const RESTRICT GetSolidVegetationTypeInformations() NOEXCEPT
	{
		return &_SolidVegetationTypeInformations;
	}

private:

	//The update task.
	Task _UpdateTask;

	//The vegetation type to update.
	VegetationType _VegetationTypeToUpdate{ VegetationType::NumberOfVegetationTypes };

	//The current camera position.
	Vector3 _CurrentCameraPosition;

	//The grass vegetation type information update.
	GrassVegetationTypeInformationUpdate _GrassVegetationTypeInformationUpdate;

	//The grass vegetation type informations.
	DynamicArray<GrassVegetationTypeInformation> _GrassVegetationTypeInformations;

	//The solid vegetation type information update.
	SolidVegetationTypeInformationUpdate _SolidVegetationTypeInformationUpdate;

	//The solid vegetation type informations.
	DynamicArray<SolidVegetationTypeInformation> _SolidVegetationTypeInformations;

	/*
	*	Processes the vegetation type information update.
	*/
	void ProcessVegetationTypeInformationUpdate() NOEXCEPT;

	/*
	*	Updates the vegetation system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Updates the grass vegetation asynchonously.
	*/
	void UpdateGrassVegetationAsynchronous() NOEXCEPT;

	/*
	*	Updates the solid vegetation asynchonously.
	*/
	void UpdateSolidVegetationAsynchronous() NOEXCEPT;

};