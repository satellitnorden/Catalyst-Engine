#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class SamplerProperties final
{

public:

	//The magnification filter.
	TextureFilter _MagnificationFilter;

	//The mipmap mode.
	MipmapMode _MipmapMode;

	//The address mode.
	AddressMode _AddressMode;

	//The number of anisotropic samples. 0 means it's disabled.
	uint8 _AnisotropicSamples;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE SamplerProperties() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE SamplerProperties(const TextureFilter initial_magnification_filter, const MipmapMode initial_mipmap_mode, const AddressMode initial_adress_mode, const uint8 initial_anisotropic_samples) NOEXCEPT
		:
		_MagnificationFilter(initial_magnification_filter),
		_MipmapMode(initial_mipmap_mode),
		_AddressMode(initial_adress_mode),
		_AnisotropicSamples(initial_anisotropic_samples)
	{

	}

	/*
	*	Equality operator overload.
	*/
	bool operator==(const SamplerProperties &other) const NOEXCEPT
	{
		return	_MagnificationFilter == other._MagnificationFilter
				&& _MipmapMode == other._MipmapMode
				&& _AddressMode == other._AddressMode
				&& _AnisotropicSamples == other._AnisotropicSamples;
	}

};