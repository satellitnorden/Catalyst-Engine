#pragma once

//Engine core.
#include <EngineCore.h>

class HeightMap final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper values..
	*/
	HeightMap() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking in the resolution of the height map.
	*/
	HeightMap(const size_t initialResolution) CATALYST_NOEXCEPT
	{
		//Resize the underlying height map data to be able to hold all the data.
		heightMapData.Resize(initialResolution * initialResolution);
	}

	/*
	*	Returns the height map value at the specified index, const.
	*/
	float At(const size_t xIndex, const size_t yIndex) const CATALYST_NOEXCEPT
	{
		return heightMapData[xIndex + yIndex];
	}

	/*
	*	Returns the height map value at the specified index, non-const.
	*/
	float& At(const size_t xIndex, const size_t yIndex) CATALYST_NOEXCEPT
	{
		return heightMapData[xIndex + yIndex];
	}

private:

	//The underlying height map data.
	DynamicArray<float> heightMapData;

};