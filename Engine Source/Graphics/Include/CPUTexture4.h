#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector4.h>

/*
*	Class representing a texture with 4 channels that can be constructed on the CPU.
*/
class CPUTexture4 final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper values.
	*/
	CPUTexture4() NOEXCEPT = delete;

	/*
	*	Constructor taking in the resolution of the texture.
	*/
	CPUTexture4(const uint32 initialResolution) NOEXCEPT
		:
		resolution(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		data.Resize(initialResolution * initialResolution);
	}

	/*
	*	Returns the texture data.
	*/
	RESTRICTED const Vector4* Data() const NOEXCEPT { return data.Data(); }

	/*
	*	Returns the texture value at the specified index, const.
	*/
	const Vector4& At(const uint32 xIndex, const uint32 yIndex) const NOEXCEPT
	{
		return data[(xIndex * resolution) + yIndex];
	}

	/*
	*	Returns the texture value at the specified index, non-const.
	*/
	Vector4& At(const uint32 xIndex, const uint32 yIndex) NOEXCEPT
	{
		return data[(xIndex * resolution) + yIndex];
	}

	/*
	*	Returns the resolution of the texture.
	*/
	uint32 GetResolution() const NOEXCEPT { return resolution; }

private:

	//The underlying texture data.
	DynamicArray<Vector4> data;

	//The resolution of the texture.
	uint32 resolution;

};