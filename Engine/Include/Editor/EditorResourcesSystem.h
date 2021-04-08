#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Building/MaterialBuildParameters.h>
#include <Resources/Building/ModelBuildParameters.h>
#include <Resources/Building/Texture2DBuildParameters.h>

class EditorResourcesSystem final
{

public:

	/*
	*	Updates the editor resources system.
	*/
	void Update() NOEXCEPT;

private:

	//Enumeration covering all create resource modes.
	enum class CreateResourceMode : uint8
	{
		NONE,
		MATERIAL,
		MODEL,
		TEXTURE_2D,
		QUIXEL_MODEL
	};

	/*
	*	Create material resource data class definition.
	*/
	class CreateMaterialResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The type.
		MaterialResource::Type _Type{ MaterialResource::Type::OPAQUE };

		//The albedo/thickness type.
		MaterialResource::MaterialResourceComponent::Type _AlbedoThicknessType{ MaterialResource::MaterialResourceComponent::Type::COLOR };

		//The albedo/thickness color.
		Vector4<float32> _AlbedoThicknessColor{ 0.5f, 0.5f, 0.5f, 1.0f };

		//The albedo/thickness texture 2D resource.
		ResourcePointer<Texture2DResource> _AlbedoThicknessTexture2DResource;

		//The normal map/displacement type.
		MaterialResource::MaterialResourceComponent::Type _NormalMapDisplacementType{ MaterialResource::MaterialResourceComponent::Type::COLOR };

		//The normal map/displacement color.
		Vector4<float32> _NormalMapDisplacementColor{ 0.5f, 0.5f, 1.0f, 0.5f };

		//The normal map/displacement texture 2D resource.
		ResourcePointer<Texture2DResource> _NormalMapDisplacementTexture2DResource;

		//The material properties type.
		MaterialResource::MaterialResourceComponent::Type _MaterialPropertiesType{ MaterialResource::MaterialResourceComponent::Type::COLOR };

		//The material properties color.
		Vector4<float32> _MaterialPropertiesColor{ 1.0f, 0.0f, 1.0f, 0.0 };

		//The material properties texture 2D resource.
		ResourcePointer<Texture2DResource> _MaterialPropertiesTexture2DResource;

		//The opacity type.
		MaterialResource::MaterialResourceComponent::Type _OpacityType{ MaterialResource::MaterialResourceComponent::Type::COLOR };

		//The opacity color.
		Vector4<float32> _OpacityColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		//The opacity texture 2D resource.
		ResourcePointer<Texture2DResource> _OpacityTexture2DResource;

		//The emissive multiplier.
		float32 _EmissiveMultiplier{ 0.0f };

		//Denotes whether or not the material is double-sided.
		bool _DoubleSided;

	};

	/*
	*	Create model resource data class definition.
	*/
	class CreateModelResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The level of detail file paths.
		DynamicArray<DynamicString> _LevelOfDetailFilepaths;

	};

	/*
	*	Create texture 2D resource data class definition.
	*/
	class CreateTexture2DResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The file 1 file path.
		DynamicString _File1FilePath;

		//The file 2 file path.
		DynamicString _File2FilePath;

		//The file 3 file path.
		DynamicString _File3FilePath;

		//The file 4 file path.
		DynamicString _File4FilePath;

		//The default.
		Vector4<float32> _Default{ 0.0f, 0.0f, 0.0f, 0.0f };

		//The channel mappings.
		StaticArray<Texture2DBuildParameters::ChannelMapping, 4> _ChannelMappings
		{
			Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED),
			Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN),
			Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE),
			Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA)
		};

		//Denotes whether or not to apply gamma correction.
		bool _ApplyGammaCorrection{ false };

		//The base mipmap level.
		int32 _BaseMipmapLevel{ 0 };

		//The number of mipmap levels.
		int32 _NumberOfMipmapLevels{ 1 };

	};

	/*
	*	Create Quixel model resource data.
	*/
	class CreateQuixelModelResourceData final
	{

	public:

		//The directory path.
		DynamicString _DirectoryPath;

	};

	//The current create resource mode.
	CreateResourceMode _CurrentCreateResourceMode{ CreateResourceMode::NONE };

	//The create material resource data.
	CreateMaterialResourceData _CreateMaterialResourceData;

	//The create model resource data.
	CreateModelResourceData _CreateModelResourceData;

	//The create texture 2D resource data.
	CreateTexture2DResourceData _CreateTexture2DResourceData;

	//The create quixel model resource data.
	CreateQuixelModelResourceData _CreateQuixelModelResourceData;

	/*
	*	Adds the create material resource window.
	*/
	void AddCreateMaterialResourceWindow() NOEXCEPT;

	/*
	*	Adds the create model resource window.
	*/
	void AddCreateModelResourceWindow() NOEXCEPT;

	/*
	*	Adds the create texture 2D resource window.
	*/
	void AddCreateTexture2DResourceWindow() NOEXCEPT;

	/*
	*	Adds the create Quixel model resource window.
	*/
	void AddCreateQuixelModelResourceWindow() NOEXCEPT;

};
#endif
