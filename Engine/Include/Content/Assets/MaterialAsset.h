#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/Asset.h>
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/Texture2DAsset.h>

//Rendering.
#include <Rendering/Native/Color.h>

class MaterialAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//Enumeration covering all types.
	enum class Type : uint8
	{
		/*
		*	A masked material will use the opacity component as the mask.
		*	Will usually run in a depth pass and a color pass.
		*/
		MASKED,

		/*
		*	An opaque material will just use the albedo, normal map and material properties components.
		*/
		OPAQUE,

		/*
		*	A translucent material will use the opacity color/texture as the opacity.
		*	Will usually run in a depth pass and a color pass.
		*/
		TRANSLUCENT
	};

	/*
	*	Component class definition.
	*/
	class Component final
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
		FORCE_INLINE Component() NOEXCEPT
		{
			
		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~Component() NOEXCEPT
		{
			if (_Type == Type::TEXTURE)
			{
				_Texture.~AssetPointer();
			}
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
	Component _AlbedoThicknessComponent;

	/*
	*	The normal map/displacement component.
	*	Expects the (tangent space) normal to be in the RGB channels, and the displacement in the A channel.
	*/
	Component _NormalMapDisplacementComponent;

	/*
	*	The material properties component.
	*	Expects the roughness to be in the R channel, the metallic in the G channel, the ambient occlusion in the B channel and the emissive in the A channel.
	*/
	Component _MaterialPropertiesComponent;

	/*
	*	The opacity component.
	*	Expects the opacity to be in the R channel.
	*/
	Component _OpacityComponent;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//Denotes whether or not this material is double-sided.
	bool _DoubleSided;

};