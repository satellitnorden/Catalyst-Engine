#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Vegetation.
#include <Vegetation/DebrisVegetationTypeInformation.h>
#include <Vegetation/GrassVegetationTypeInformation.h>
#include <Vegetation/SolidVegetationTypeInformation.h>
#include <Vegetation/TreeVegetationTypeInformation.h>
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
	*	Updates the vegetation system sequentially.
	*/
	void SequentialUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Adds a debris vegetation type.
	*/
	void AddDebrisVegetationType(const DebrisVegetationTypeProperties &properties, const PhysicalModel &model, const PhysicalMaterial &material) NOEXCEPT;

	/*
	*	Returns the debris vegetation type informations, const.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<DebrisVegetationTypeInformation> *const RESTRICT GetDebrisVegetationTypeInformations() const NOEXCEPT
	{
		return &_DebrisVegetationTypeInformations;
	}

	/*
	*	Returns the debris vegetation type informations, non-const.
	*/
	RESTRICTED NO_DISCARD DynamicArray<DebrisVegetationTypeInformation> *const RESTRICT GetDebrisVegetationTypeInformations() NOEXCEPT
	{
		return &_DebrisVegetationTypeInformations;
	}

	/*
	*	Adds a grass vegetation type.
	*/
	void AddGrassVegetationType(const GrassVegetationTypeProperties &properties, const GrassVegetationModel &model, const GrassVegetationMaterial &material) NOEXCEPT;

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
	void AddSolidVegetationType(const SolidVegetationTypeProperties &properties, const PhysicalModel &model, const PhysicalMaterial &material) NOEXCEPT;

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

	/*
	*	Adds a tree vegetation type.
	*/
	void AddTreeVegetationType(const TreeVegetationTypeProperties &properties, const TreeVegetationModel &model, const TreeVegetationMaterial &material) NOEXCEPT;

	/*
	*	Returns the tree vegetation type informations, const.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<TreeVegetationTypeInformation> *const RESTRICT GetTreeVegetationTypeInformations() const NOEXCEPT
	{
		return &_TreeVegetationTypeInformations;
	}

	/*
	*	Returns the tree vegetation type informations, non-const.
	*/
	RESTRICTED NO_DISCARD DynamicArray<TreeVegetationTypeInformation> *const RESTRICT GetTreeVegetationTypeInformations() NOEXCEPT
	{
		return &_TreeVegetationTypeInformations;
	}

private:

	//The update task.
	Task _UpdateTask;

	//The vegetation type to update.
	VegetationType _VegetationTypeToUpdate{ VegetationType::NumberOfVegetationTypes };

	//The debris vegetation type information update.
	VegetationTypeInformationUpdate<DebrisVegetationTypeInformation, VegetationPatchInformation, DebrisVegetationPatchRenderInformation> _DebrisVegetationTypeInformationUpdate;

	//The debris vegetation type informations.
	DynamicArray<DebrisVegetationTypeInformation> _DebrisVegetationTypeInformations;

	//The grass vegetation type information update.
	VegetationTypeInformationUpdate<GrassVegetationTypeInformation, VegetationPatchInformation, GrassVegetationPatchRenderInformation> _GrassVegetationTypeInformationUpdate;

	//The grass vegetation type informations.
	DynamicArray<GrassVegetationTypeInformation> _GrassVegetationTypeInformations;

	//The solid vegetation type information update.
	VegetationTypeInformationUpdate<SolidVegetationTypeInformation, VegetationPatchInformation, SolidVegetationPatchRenderInformation> _SolidVegetationTypeInformationUpdate;

	//The solid vegetation type informations.
	DynamicArray<SolidVegetationTypeInformation> _SolidVegetationTypeInformations;

	//The tree vegetation type information update.
	VegetationTypeInformationUpdate<TreeVegetationTypeInformation, TreeVegetationPatchInformation, TreeVegetationPatchRenderInformation> _TreeVegetationTypeInformationUpdate;

	//The tree vegetation type informations.
	DynamicArray<TreeVegetationTypeInformation> _TreeVegetationTypeInformations;

	/*
	*	Processes the vegetation type information update.
	*/
	void ProcessVegetationTypeInformationUpdate() NOEXCEPT;

	/*
	*	Updates the vegetation system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

};