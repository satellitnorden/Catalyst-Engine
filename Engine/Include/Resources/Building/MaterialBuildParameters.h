#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/Color.h>

class MaterialBuildParameters final
{

public:

	/*
	*	Material build parameter component class definition.
	*/
	class MaterialBuildParameterComponent final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			COLOR,
			TEXTURE
		};

		//The type.
		Type _Type;

		union
		{
			//The color.
			Color _Color;

			//The texture resource identifier.
			HashString _TextureResourceIdentifier;
		};

	};

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The albedo component.
	MaterialBuildParameterComponent _AlbedoComponent;

	//The normal map component.
	MaterialBuildParameterComponent _NormalMapComponent;

	//The material properties component.
	MaterialBuildParameterComponent _MaterialPropertiesComponent;

};