#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Resources.
#include <Resources/Core/MaterialResource.h>

class ShaderMaterial final
{

public:

	/*
	*	The properties.
	*	Maps the the CatalystShaderConstants::MATERIAL_PROPERTY_X constants.
	*/
	uint32 _Properties;

	/*
	*	The albedo/thickness.
	*	Can be either an RGBA color encoded in 8 bits each, or a texture index.
	*/
	uint32 _AlbedoThickness;

	/*
	*	The normal map/displacement.
	*	Can be either an RGBA color encoded in 8 bits each, or a texture index.
	*/
	uint32 _NormalMapDisplacement;

	/*
	*	The material properties.
	*	Can be either an RGBA color encoded in 8 bits each, or a texture index.
	*/
	uint32 _MaterialProperties;

	/*
	*	The opacity.
	*	Can be either an RGBA color encoded in 8 bits each, or a texture index.
	*/
	uint32 _Opacity;

	/*
	*	The emissive multiplier.
	*/
	float32 _EmissiveMultiplier;

	/*
	*	Some padding.
	*	Needed because each material needs to be a multiple of 16 bytes in size for the GPU.
	*/
	Padding<8> _Padding;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ShaderMaterial() NOEXCEPT
	{

	}

	/*
	*	Constructor taking a material resource.
	*/
	FORCE_INLINE ShaderMaterial(const MaterialResource *const RESTRICT resource) NOEXCEPT
	{
		//The first 3 bits in _Properties are reserved to encode the type, so set that first.
		switch (resource->_Type)
		{
			case MaterialResource::Type::MASKED:
			{
				_Properties = CatalystShaderConstants::MATERIAL_PROPERTY_TYPE_MASKED;

				break;
			}

			case MaterialResource::Type::OPAQUE:
			{
				_Properties = CatalystShaderConstants::MATERIAL_PROPERTY_TYPE_OPAQUE;

				break;
			}

			case MaterialResource::Type::TRANSLUCENT:
			{
				_Properties = CatalystShaderConstants::MATERIAL_PROPERTY_TYPE_TRANSLUCENT;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the albedo/thickness.
		switch (resource->_AlbedoThicknessComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				_AlbedoThickness = *resource->_AlbedoThicknessComponent._Color.Data();

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE);
				_AlbedoThickness = resource->_AlbedoThicknessComponent._Texture->_Index;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the normal map/displacement.
		switch (resource->_NormalMapDisplacementComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				_NormalMapDisplacement = *resource->_NormalMapDisplacementComponent._Color.Data();

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE);
				_NormalMapDisplacement = resource->_NormalMapDisplacementComponent._Texture->_Index;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the material properties.
		switch (resource->_MaterialPropertiesComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				_MaterialProperties = *resource->_MaterialPropertiesComponent._Color.Data();

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE);
				_MaterialProperties = resource->_MaterialPropertiesComponent._Texture->_Index;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the opacity.
		switch (resource->_OpacityComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				_Opacity = *resource->_OpacityComponent._Color.Data();

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_OPACITY_TEXTURE);
				_Opacity = resource->_OpacityComponent._Texture->_Index;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Copy the emissive multiplier.
		_EmissiveMultiplier = resource->_EmissiveMultiplier;
	}

};

static_assert(sizeof(ShaderMaterial) == 32, "ShaderMaterial size must be a multiple of 16!");