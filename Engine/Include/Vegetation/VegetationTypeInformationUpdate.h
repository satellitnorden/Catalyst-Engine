#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Vegetation.
#include <Vegetation/SolidVegetationTypeInformation.h>

template <typename TYPE_INFORMATION_TYPE, typename PATCH_INFORMATION_TYPE, typename PATCH_RENDER_INFORMATION_TYPE>
class VegetationTypeInformationUpdate final
{

public:

	//The vegetation type information to update.
	TYPE_INFORMATION_TYPE *RESTRICT _Information;

	//Denotes whether or not this is a level of detail update.
	bool _LevelOfDetailUpdate;

	//The index.
	uint8 _Index;

	//The patch indices to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//The new patch information.
	PATCH_INFORMATION_TYPE _NewPatchInformation;

	//The new patch render information.
	PATCH_RENDER_INFORMATION_TYPE _NewPatchRenderInformation;

};