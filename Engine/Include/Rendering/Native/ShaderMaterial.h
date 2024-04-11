#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Content.
#include <Content/Assets/MaterialAsset.h>

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
	FORCE_INLINE ShaderMaterial(const MaterialAsset *const RESTRICT material) NOEXCEPT
	{
		//The first 3 bits in _Properties are reserved to encode the type, so set that first.
		switch (material->_Type)
		{
			case MaterialAsset::Type::MASKED:
			{
				_Properties = CatalystShaderConstants::MATERIAL_PROPERTY_TYPE_MASKED;

				break;
			}

			case MaterialAsset::Type::OPAQUE:
			{
				_Properties = CatalystShaderConstants::MATERIAL_PROPERTY_TYPE_OPAQUE;

				break;
			}

			case MaterialAsset::Type::TRANSLUCENT:
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
		switch (material->_AlbedoThicknessComponent._Type)
		{
			case MaterialAsset::Component::Type::COLOR:
			{
				CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE);
				_AlbedoThickness = *material->_AlbedoThicknessComponent._Color.Data();

				break;
			}

			case MaterialAsset::Component::Type::TEXTURE:
			{
				if (true) //Placeholder for if texture is loaded.
				{
					SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE);
					_AlbedoThickness = material->_AlbedoThicknessComponent._Texture->_Index;
				}
				
				else
				{
					CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE);
					_AlbedoThickness = *Color(material->_AlbedoThicknessComponent._Texture->_AverageValue).Data();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the normal map/displacement.
		switch (material->_NormalMapDisplacementComponent._Type)
		{
			case MaterialAsset::Component::Type::COLOR:
			{
				CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE);
				_NormalMapDisplacement = *material->_NormalMapDisplacementComponent._Color.Data();

				break;
			}

			case MaterialAsset::Component::Type::TEXTURE:
			{
				if (true) //Placeholder for if texture is loaded.
				{
					SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE);
					_NormalMapDisplacement = material->_NormalMapDisplacementComponent._Texture->_Index;
				}

				else
				{
					CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE);
					_NormalMapDisplacement = *Color(material->_NormalMapDisplacementComponent._Texture->_AverageValue).Data();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the material properties.
		switch (material->_MaterialPropertiesComponent._Type)
		{
			case MaterialAsset::Component::Type::COLOR:
			{
				CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE);
				_MaterialProperties = *material->_MaterialPropertiesComponent._Color.Data();

				break;
			}

			case MaterialAsset::Component::Type::TEXTURE:
			{
				if (true) //Placeholder for if texture is loaded.
				{
					SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE);
					_MaterialProperties = material->_MaterialPropertiesComponent._Texture->_Index;
				}

				else
				{
					CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE);
					_MaterialProperties = *Color(material->_MaterialPropertiesComponent._Texture->_AverageValue).Data();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set the opacity.
		switch (material->_OpacityComponent._Type)
		{
			case MaterialAsset::Component::Type::COLOR:
			{
				CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_OPACITY_TEXTURE);
				_Opacity = *material->_OpacityComponent._Color.Data();

				break;
			}

			case MaterialAsset::Component::Type::TEXTURE:
			{
				if (true) //Placeholder for if texture is loaded.
				{
					SET_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_OPACITY_TEXTURE);
					_Opacity = material->_OpacityComponent._Texture->_Index;
				}

				else
				{
					CLEAR_BIT(_Properties, CatalystShaderConstants::MATERIAL_PROPERTY_OPACITY_TEXTURE);
					_Opacity = *Color(material->_OpacityComponent._Texture->_AverageValue).Data();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Copy the emissive multiplier.
		_EmissiveMultiplier = material->_EmissiveMultiplier;
	}

};

static_assert(sizeof(ShaderMaterial) == 32, "ShaderMaterial size must be a multiple of 16!");