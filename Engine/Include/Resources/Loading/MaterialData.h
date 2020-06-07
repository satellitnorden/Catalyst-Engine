#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/MaterialResource.h>

class MaterialData final
{

public:

	/*
	*	Material data component class definition.
	*/
	class MaterialDataComponent final
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
		FORCE_INLINE MaterialDataComponent() NOEXCEPT
		{

		}

	};

	//The type.
	MaterialResource::Type _Type;

	//The albedo/thickness component.
	MaterialDataComponent _AlbedoThicknessComponent;

	//The normal map/displacement component.
	MaterialDataComponent _NormalMapDisplacementComponent;

	//The material properties component.
	MaterialDataComponent _MaterialPropertiesComponent;

	//The opacity component.
	MaterialDataComponent _OpacityComponent;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//Denotes whether or not this material is double-sided.
	bool _DoubleSided;

};