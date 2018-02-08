#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector4.h>

class NormalMap final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper values..
	*/
	NormalMap() NOEXCEPT = delete;

	/*
	*	Constructor taking in the resolution of the height map.
	*/
	NormalMap(const size_t initialResolution) NOEXCEPT
		:
		normalMapResolution(initialResolution)
	{
		//Resize the underlying normal map data to be able to hold all the data.
		normalMapData.Resize(initialResolution * initialResolution);
	}

	/*
	*	Returns the normal map data.
	*/
	RESTRICTED const Vector4* Data() const NOEXCEPT { return normalMapData.Data(); }

	/*
	*	Returns the normal map value at the specified index, const.
	*/
	const Vector4& At(const size_t xIndex, const size_t yIndex) const NOEXCEPT
	{
		return normalMapData[xIndex + yIndex];
	}

	/*
	*	Returns the height map value at the specified index, non-const.
	*/
	Vector4& At(const size_t xIndex, const size_t yIndex) NOEXCEPT
	{
		return normalMapData[xIndex + yIndex];
	}

	/*
	*	Returns the resolution of the normal map.
	*/
	size_t GetResolution() const NOEXCEPT { return normalMapResolution; }

private:

	//The underlying normal map data.
	DynamicArray<Vector4> normalMapData;

	//The resolution of the normal map.
	size_t normalMapResolution;

};