#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/Texture2DAsset.h>

//Rendering.
#include <Rendering/Native/Color.h>

//Resources.
#include <Resources/Core/Resource.h>

class MaterialResource final : public Resource
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		/*
		*	Visual entities with a masked material will use the opacity color/texture as the mask.
		*	Will usually run in a depth pass and a color pass.
		*/
		MASKED,

		/*
		*	Visual entities with an opaque material will just use the albedo, normal map and material properties textures.
		*/
		OPAQUE,

		/*
		*	Visual entities with a translucent material will use the opacity color/texture as the opacity.
		*	Will usually run in a depth pass and a color pass.
		*/
		TRANSLUCENT
	};

	/*
	*	Material resource component class definition.
	*/
	class MaterialResourceComponent final
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

			//The texture
			AssetPointer<Texture2DAsset> _Texture;
		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE MaterialResourceComponent() NOEXCEPT
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~MaterialResourceComponent() NOEXCEPT
		{

		}

	};

	//The index.
	uint32 _Index;

	//The type.
	Type _Type;

	/*
	*	The albedo/thickness component.
	*	Expects the albedo to be in the RGB channels, and the thickness to be in the A channel.
	*/
	MaterialResourceComponent _AlbedoThicknessComponent;

	/*
	*	The normal map/displacement component.
	*	Expects the (tangent space) normal to be in the RGB channels, and the displacement in the A channel.
	*/
	MaterialResourceComponent _NormalMapDisplacementComponent;

	/*
	*	The material properties component.
	*	Expects the roughness to be in the R channel, the metallic in the G channel, the ambient occlusion in the B channel and the emissive in the A channel.
	*/
	MaterialResourceComponent _MaterialPropertiesComponent;

	/*
	*	The opacity component.
	*	Expects the opacity to be in the R channel.
	*/
	MaterialResourceComponent _OpacityComponent;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//Denotes whether or not this material is double-sided.
	bool _DoubleSided;

};