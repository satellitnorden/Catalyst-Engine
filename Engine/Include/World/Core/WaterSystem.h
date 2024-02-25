#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class WaterSystem final
{

public:

	/*
	*	Initializes the water system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the water system during render.
	*/
	void RenderUpdate() NOEXCEPT;

private:

	/*
	*	Water uniform data class definition.
	*/
	class WaterUniformData final
	{

	public:

		//The offsets
		StaticArray<Vector2<float32>, 4> _Offsets;

	};

	//The Water uniform data.
	WaterUniformData _WaterUniformData;

};