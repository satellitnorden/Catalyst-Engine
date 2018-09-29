#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Vegetation.
#include <Vegetation/VegetationAddTypeInformation.h>
#include <Vegetation/VegetationTypeInformation.h>

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
	*	Updates the vegetation system synchronously during the closing update phase.
	*/
	void OpeningUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Adds a vegetation type.
	*/
	void AddVegetationType(const VegetationAddTypeInformation &addInformation) NOEXCEPT;

private:

	//The vegetation type informations.
	DynamicArray<VegetationTypeInformation> _VegetationTypeInformations;

};