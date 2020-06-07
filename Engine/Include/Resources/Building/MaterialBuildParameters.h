#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/Color.h>

//Resources.
#include <Resources/Core/MaterialResource.h>

class MaterialBuildParameters final
{

public:

	/*
	*	Material build parameter component class definition.
	*/
	class MaterialBuildParameterComponent final
	{

	public:

		//The type.
		MaterialResource::MaterialResourceComponent::Type _Type;

		union
		{
			//The color.
			Color _Color;

			//The texture resource identifier.
			HashString _TextureResourceIdentifier;
		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE MaterialBuildParameterComponent() NOEXCEPT
		{

		}

	};

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The type.
	MaterialResource::Type _Type;

	//The albedo/thickness component.
	MaterialBuildParameterComponent _AlbedoThicknessComponent;

	//The normal map/displacement component.
	MaterialBuildParameterComponent _NormalMapDisplacementComponent;

	//The material properties component.
	MaterialBuildParameterComponent _MaterialPropertiesComponent;

	//The opacity component.
	MaterialBuildParameterComponent _OpacityComponent;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//Denotes whether or not this material is double-sided.
	bool _DoubleSided;

};