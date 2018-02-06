#pragma once

//Engine core.
#include <EngineCore.h>

class HeightMap final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper values..
	*/
	HeightMap() NOEXCEPT = delete;

	/*
	*	Constructor taking in the resolution of the height map.
	*/
	HeightMap(const size_t initialResolution) NOEXCEPT
		:
		heightMapResolution(initialResolution)
	{
		//Resize the underlying height map data to be able to hold all the data.
		heightMapData.Resize(initialResolution * initialResolution);
	}

	/*
	*	Returns the height map data.
	*/
	RESTRICTED const float* Data() const NOEXCEPT { return heightMapData.Data(); }

	/*
	*	Returns the height map value at the specified index, const.
	*/
	float At(const size_t xIndex, const size_t yIndex) const NOEXCEPT
	{
		return heightMapData[xIndex + yIndex];
	}

	/*
	*	Returns the height map value at the specified index, non-const.
	*/
	float& At(const size_t xIndex, const size_t yIndex) NOEXCEPT
	{
		return heightMapData[xIndex + yIndex];
	}

	/*
	*	Returns the resolution of the height map.
	*/
	size_t GetResolution() const NOEXCEPT { return heightMapResolution; }

private:

	//The underlying height map data.
	DynamicArray<float> heightMapData;

	//The resolution of the height map.
	size_t heightMapResolution;

};