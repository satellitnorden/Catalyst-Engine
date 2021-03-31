#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>

//World.
#include <World/Vegetation/ProceduralGrassProperties.h>

class VegetationSystem final
{

public:

	/*
	*	Enables procedural grass.
	*/
	void EnableProceduralGrass(const ProceduralGrassProperties &properties) NOEXCEPT;

	/*
	*	Returns whether or not procedural grass is enabled.
	*/
	FORCE_INLINE NO_DISCARD bool IsProceduralGrassEnabled() const NOEXCEPT
	{
		return static_cast<bool>(_ProceduralGrassProperties);
	}

	/*
	*	Returns the procedural grass properties.
	*/
	FORCE_INLINE NO_DISCARD const ProceduralGrassProperties &GetProceduralGrassProperties() const NOEXCEPT
	{
		return _ProceduralGrassProperties.Get();
	}

private:

	//The procedural grass properties.
	Optional<ProceduralGrassProperties> _ProceduralGrassProperties;

};