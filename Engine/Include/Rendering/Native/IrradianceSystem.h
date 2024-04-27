#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class IrradianceSystem final
{

public:

	/*
	*	Post initializes the irradiance system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the irradiance system during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

private:

	/*
	*	Irradiance uniform data class definition.
	*/
	class IrradianceUniformData final
	{

	public:

		//The irradiance hemisphere directions.
		StaticArray<Vector4<float32>, 64> _IrradianceHemisphereDirections;

	};

	//The uniform data.
	IrradianceUniformData _UniformData;

};