//Header file.
#include <Systems/RenderingSystem.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>
#include <Entities/Creation/ParticleSystemInitializationData.h>
#include <Entities/Types/Entity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CommonEnvironmentMaterialData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/CommonOceanMaterialData.h>
#endif
#include <Rendering/Engine/CommonParticleMaterialData.h>
#include <Rendering/Engine/CommonPhysicalMaterialData.h>
#include <Rendering/Engine/CommonPhysicalModelData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/OceanMaterial.h>
#endif
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/TextureData.h>
#include <Rendering/Engine/Viewer.h>
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/GrassVegetationMaterialData.h>
#include <Resources/GrassVegetationModelData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Resources/OceanMaterialData.h>
#endif
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/TerrainMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/InputSystem.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Define the current rendering system.
#if defined(CATALYST_WINDOWS) || defined(CATALYST_ANDROID)
	#define CURRENT_RENDERING_SYSTEM VulkanRenderingSystem
#endif

/*
*	Initializes the rendering system.
*/
void RenderingSystem::InitializeSystem(const CatalystProjectRenderingConfiguration &configuration) NOEXCEPT
{
	//Set the resolution.
	_Resolution = configuration._Resolution;

	//Set the scaled resolution.
	_ScaledResolution = Resolution(static_cast<uint32>(static_cast<float>(_Resolution._Width) * configuration._ResolutionScale), static_cast<uint32>(static_cast<float>(_Resolution._Height) * configuration._ResolutionScale));

	//Set the directional shadow map resolution.
	_DirectionalShadowMapResolution = Resolution(configuration._ShadowMapResolution, configuration._ShadowMapResolution);

	//Pre-initialize the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->PreInitializeSystem();

	//Initialize all depth buffers.
	InitializeDepthBuffers();

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all samplers.
	InitializeSamplers();

	//Initialize all common render data table layouts.
	InitializeCommonRenderDataTableLayouts();

	//Initialize the common environment materials.
	InitializeCommonEnvironmentMaterials();

#if defined(CATALYST_ENABLE_OCEAN)
	//Initialize the common ocean materials.
	InitializeCommonOceanMaterials();
#endif

	//Initialize the common particle materials.
	InitializeCommonParticleMaterials();

	//Initializes the common physical materials.
	InitializeCommonPhysicalMaterials();

	//Initialize the common physical models.
	InitializeCommonPhysicalModels();

	//Post-initialize the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->PostInitializeSystem();
}

/*
*	Post-initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem()
{
	//Register all render passes.
	RegisterRenderPasses();

	//Initialize all render passes.
	InitializeRenderPasses();
}

#include <Systems/TerrainSystem.h>

/*
*	Updates the rendering system synchronously during the rendering update phase.
*/
void RenderingSystem::RenderingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//TEMP: Update the terrain material.
	TerrainMaterial material;
	TerrainSystem::Instance->GetTerrainProperties()->_PatchPropertiesGenerationFunction(*TerrainSystem::Instance->GetTerrainProperties(), Vector3(0.0f, 0.0f, 0.0f), &material);
	TerrainSystem::Instance->GetTerrainProperties()->_RenderDataTable = material._RenderDataTable;

	//Render-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PreUpdateSystemSynchronous();

	//Render all render passes.
	for (RenderPass *const RESTRICT _RenderPass : _RenderPasses)
	{
		_RenderPass->RenderAsynchronous();
	}

	//Post-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PostUpdateSystemSynchronous();
}

/*
*	Releases the rendering system.
*/
void RenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->ReleaseSystem();
}

/*
*	Returns the current frame index.
*/
uint8 RenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	//Return the current frame index via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentFrameIndex();
}

/*
*	Returns a sampler with the given sampler properties.
*/
SamplerHandle RenderingSystem::GetSampler(const SamplerProperties &properties) const NOEXCEPT
{
#if !defined(CATALYST_FINAL)
	const SamplerHandle *const RESTRICT sampler{ _Samplers.Find(properties) };

	ASSERT(sampler, "A sampler with these sampler properties needs to be created!");

	return *sampler;
#else
	return *_Samplers.Find(properties);
#endif
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle RenderingSystem::CreateConstantBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	//Create the constant buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateConstantBuffer(data, dataSizes, dataChunks);
}

/*
*	Destroys a constant buffer.
*/
void RenderingSystem::DestroyConstantBuffer(ConstantBufferHandle handle) const NOEXCEPT
{
	//Create the constant buffer via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyConstantBuffer(handle);
}

/*
*	Creates a depth buffer.
*/
DepthBufferHandle RenderingSystem::CreateDepthBuffer(const Resolution resolution) const NOEXCEPT
{
	//Create the depth buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateDepthBuffer(resolution);
}

/*
*	Creates a render data table layout.
*/
void RenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render data table layout via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateRenderDataTableLayout(bindings, numberOfBindings, handle);
}

/*
*	Creates a render data table.
*/
void RenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateRenderDataTable(renderDataTableLayout, handle);
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	//Bind the combined image sampler to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindCombinedImageSamplerToRenderDataTable(binding, renderDataTable, image, sampler);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void RenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT
{
	//Bind the uniform buffer to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindUniformBufferToRenderDataTable(binding, renderDataTable, uniformBuffer);
}

/*
*	Destroys a render data table.
*/
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle handle) const NOEXCEPT
{
	//Destroy the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyRenderDataTable(handle);
}

/*
*	Creates a render target.
*/
void RenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, const TextureFilter filter, const AddressMode addressMode, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateRenderTarget(resolution, format, filter, addressMode, handle);
}

/*
*	Creates and returns a sampler.
*/
SamplerHandle RenderingSystem::CreateSampler(const SamplerProperties &properties) const NOEXCEPT
{
	//Create the sampler via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateSampler(properties);
}

/*
*	Creates and returns a texture 2D given the texture data.
*/
Texture2DHandle RenderingSystem::CreateTexture2D(const TextureData &textureData) const NOEXCEPT
{
	//Create the texture 2D via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->Create2DTexture(textureData);
}

/*
*	Destroys a texture 2D
*/
void RenderingSystem::DestroyTexture2D(Texture2DHandle handle) const NOEXCEPT
{
	//Destroy the texture 2D via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyTexture2D(handle);
}

/*
*	Creates a texture cube.
*/
TextureCubeHandle RenderingSystem::CreateTextureCube(const float *const RESTRICT data, const Resolution resolution) const NOEXCEPT
{
	//Create the texture cube via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateTextureCube(data, resolution);
}

/*
*	Creates and returns a uniform buffer.
*/
UniformBufferHandle RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	//Create the uniform buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Uploads data to a uniform buffer.
*/
void RenderingSystem::UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT
{
	//Upload the data to the uniform buffer via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->UploadDataToUniformBuffer(handle, data);
}

/*
*	Destroys a uniform buffer.
*/
void RenderingSystem::DestroyUniformBuffer(UniformBufferHandle handle) const NOEXCEPT
{
	//Destroy the uniform buffer via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyUniformBuffer(handle);
}

/*
*	Returns the current dynamic uniform data render data table.
*/
RenderDataTableHandle RenderingSystem::GetCurrentDynamicUniformDataRenderDataTable() const NOEXCEPT
{
	//Return the current dynamic uniform data render data table via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentDynamicUniformDataRenderDataTable();
}

/*
*	Returns the given common render data table layout.
*/
RenderDataTableHandle RenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT
{
	//Return the given common render data table layout.
	return _CommonRenderDataTableLayouts[UNDERLYING(commonRenderDataTableLayout)];
}

/*
*	Creates an environment material.
*/
void RenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &data, EnvironmentMaterial &material) NOEXCEPT
{
	//Create the diffuse texture.
	material._DiffuseTexture = CreateTextureCube(data._DiffuseData.Data(), Resolution(data._DiffuseResolution, data._DiffuseResolution));

	//Create the diffuse irradiance texture.
	material._DiffuseIrradianceTexture = CreateTextureCube(data._DiffuseIrradianceData.Data(), Resolution(data._DiffuseIrradianceResolution, data._DiffuseIrradianceResolution));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::EnvironmentMaterial), &material._RenderDataTable);

	//Update the render data table.
	BindCombinedImageSamplerToRenderDataTable(0, material._RenderDataTable, material._DiffuseTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(1, material._RenderDataTable, material._DiffuseIrradianceTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat)));
}

/*
*	Creates a grass vegetation material.
*/
void RenderingSystem::CreateGrassVegetationMaterial(const GrassVegetationMaterialData &data, GrassVegetationMaterial &material) NOEXCEPT
{
	//Create the mask texture.
	material._MaskTexture = CreateTexture2D(TextureData(TextureDataContainer(data._MaskData, data._MaskWidth, data._MaskHeight, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the albedo texture.
	material._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(data._AlbedoData, data._Width, data._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	material._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(data._NormalMapData, data._Width, data._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial), &material._RenderDataTable);
	BindCombinedImageSamplerToRenderDataTable(0, material._RenderDataTable, material._MaskTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge)));
	BindCombinedImageSamplerToRenderDataTable(1, material._RenderDataTable, material._AlbedoTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge)));
	BindCombinedImageSamplerToRenderDataTable(2, material._RenderDataTable, material._NormalMapTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge)));
}

/*
*	Creates a grass vegetation model.
*/
void RenderingSystem::CreateGrassVegetationModel(const GrassVegetationModelData &data, GrassVegetationModel &model) NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ data._Vertices.Data(), data._Indices.Data() };
	const uint64 modelDataSizes[]{ sizeof(GrassVegetationVertex) * data._Vertices.Size(), sizeof(uint32) * data._Indices.Size() };
	ConstantBufferHandle buffer = CreateConstantBuffer(modelData, modelDataSizes, 2);

	//Set up the physical model.
	model._AxisAlignedBoundingBox._Minimum = Vector3(-data._Extent * 0.5f, -data._Extent * 0.5f, -data._Extent * 0.5f);
	model._AxisAlignedBoundingBox._Maximum = Vector3(data._Extent * 0.5f, data._Extent * 0.5f, data._Extent * 0.5f);
	model._Buffer = buffer;
	model._IndexOffset = modelDataSizes[0];
	model._IndexCount = static_cast<uint32>(data._Indices.Size());
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Creates an ocean material.
*/
void RenderingSystem::CreateOceanMaterial(const OceanMaterialData &data, OceanMaterial &material) const NOEXCEPT
{
	//Create the normal texture.
	material._NormalTexture = CreateTexture2D(TextureData(TextureDataContainer(data._NormalData, data._Width, data._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the foam texture.
	material._FoamTexture = CreateTexture2D(TextureData(TextureDataContainer(data._FoamData, data._Width, data._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::OceanMaterial), &material._RenderDataTable);

	//Update the render data table.
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, material._RenderDataTable, material._NormalTexture, RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, material._RenderDataTable, material._FoamTexture, RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
}
#endif

/*
*	Creates a physical model.
*/
void RenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ physicalModelData._Vertices.Data(), physicalModelData._Indices.Data() };
	const uint64 modelDataSizes[]{ sizeof(PhysicalVertex) * physicalModelData._Vertices.Size(), sizeof(uint32) * physicalModelData._Indices.Size() };
	ConstantBufferHandle buffer = CreateConstantBuffer(modelData, modelDataSizes, 2);

	//Set up the physical model.
	physicalModel._AxisAlignedBoundingBox._Minimum = Vector3(-physicalModelData._Extent * 0.5f, -physicalModelData._Extent * 0.5f, -physicalModelData._Extent * 0.5f);
	physicalModel._AxisAlignedBoundingBox._Maximum = Vector3(physicalModelData._Extent * 0.5f, physicalModelData._Extent * 0.5f, physicalModelData._Extent * 0.5f);
	physicalModel._Buffer = buffer;
	physicalModel._IndexOffset = modelDataSizes[0];
	physicalModel._IndexCount = static_cast<uint32>(physicalModelData._Indices.Size());
}

/*
*	Creates a particle material.
*/
void RenderingSystem::CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT
{
	//Create the albedo texture
	particleMaterial._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(particleMaterialData._AlbedoData, particleMaterialData._Width, particleMaterialData._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Create the albedo texture.
	physicalMaterial._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._AlbedoData, physicalMaterialData._Width, physicalMaterialData._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	physicalMaterial._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._NormalMapData, physicalMaterialData._Width, physicalMaterialData._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the material properties texture.
	physicalMaterial._MaterialPropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._MaterialPropertiesData, physicalMaterialData._Width, physicalMaterialData._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::PhysicalMaterial), &physicalMaterial._RenderDataTable);
	BindCombinedImageSamplerToRenderDataTable(1, physicalMaterial._RenderDataTable, physicalMaterial._AlbedoTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(2, physicalMaterial._RenderDataTable, physicalMaterial._NormalMapTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(3, physicalMaterial._RenderDataTable, physicalMaterial._MaterialPropertiesTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
}

/*
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the first layer albedo.
	terrainMaterial._FirstLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FirstLayerAlbedoData, terrainMaterialData._FirstLayerWidth, terrainMaterialData._FirstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer normal map.
	terrainMaterial._FirstLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FirstLayerNormalMapData, terrainMaterialData._FirstLayerWidth, terrainMaterialData._FirstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the first layer material properties.
	terrainMaterial._FirstLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FirstLayerMaterialPropertiesData, terrainMaterialData._FirstLayerWidth, terrainMaterialData._FirstLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer albedo.
	terrainMaterial._SecondLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._SecondLayerAlbedoData, terrainMaterialData._SecondLayerWidth, terrainMaterialData._SecondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer normal map.
	terrainMaterial._SecondLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._SecondLayerNormalMapData, terrainMaterialData._SecondLayerWidth, terrainMaterialData._SecondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the second layer material properties.
	terrainMaterial._SecondLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._SecondLayerMaterialPropertiesData, terrainMaterialData._SecondLayerWidth, terrainMaterialData._SecondLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer albedo.
	terrainMaterial._ThirdLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._ThirdLayerAlbedoData, terrainMaterialData._ThirdLayerWidth, terrainMaterialData._ThirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer normal map.
	terrainMaterial._ThirdLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._ThirdLayerNormalMapData, terrainMaterialData._ThirdLayerWidth, terrainMaterialData._ThirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the third layer material properties.
	terrainMaterial._ThirdLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._ThirdLayerMaterialPropertiesData, terrainMaterialData._ThirdLayerWidth, terrainMaterialData._ThirdLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer albedo.
	terrainMaterial._FourthLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FourthLayerAlbedoData, terrainMaterialData._FourthLayerWidth, terrainMaterialData._FourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer normal map.
	terrainMaterial._FourthLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FourthLayerNormalMapData, terrainMaterialData._FourthLayerWidth, terrainMaterialData._FourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fourth layer material properties.
	terrainMaterial._FourthLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FourthLayerMaterialPropertiesData, terrainMaterialData._FourthLayerWidth, terrainMaterialData._FourthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer albedo.
	terrainMaterial._FifthLayerAlbedo = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FifthLayerAlbedoData, terrainMaterialData._FifthLayerWidth, terrainMaterialData._FifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer normal map.
	terrainMaterial._FifthLayerNormalMap = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FifthLayerNormalMapData, terrainMaterialData._FifthLayerWidth, terrainMaterialData._FifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the fifth layer material properties.
	terrainMaterial._FifthLayerMaterialProperties = CreateTexture2D(TextureData(TextureDataContainer(terrainMaterialData._FifthLayerMaterialPropertiesData, terrainMaterialData._FifthLayerWidth, terrainMaterialData._FifthLayerHeight, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::TerrainMaterial), &terrainMaterial._RenderDataTable);
	BindCombinedImageSamplerToRenderDataTable(0, terrainMaterial._RenderDataTable, terrainMaterial._FirstLayerAlbedo, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(1, terrainMaterial._RenderDataTable, terrainMaterial._FirstLayerNormalMap, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(2, terrainMaterial._RenderDataTable, terrainMaterial._FirstLayerMaterialProperties, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(3, terrainMaterial._RenderDataTable, terrainMaterial._SecondLayerAlbedo, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(4, terrainMaterial._RenderDataTable, terrainMaterial._SecondLayerNormalMap, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(5, terrainMaterial._RenderDataTable, terrainMaterial._SecondLayerMaterialProperties, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(6, terrainMaterial._RenderDataTable, terrainMaterial._ThirdLayerAlbedo, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(7, terrainMaterial._RenderDataTable, terrainMaterial._ThirdLayerNormalMap, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(8, terrainMaterial._RenderDataTable, terrainMaterial._ThirdLayerMaterialProperties, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(9, terrainMaterial._RenderDataTable, terrainMaterial._FourthLayerAlbedo, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(10, terrainMaterial._RenderDataTable, terrainMaterial._FourthLayerNormalMap, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(11, terrainMaterial._RenderDataTable, terrainMaterial._FourthLayerMaterialProperties, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(12, terrainMaterial._RenderDataTable, terrainMaterial._FifthLayerAlbedo, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(13, terrainMaterial._RenderDataTable, terrainMaterial._FifthLayerNormalMap, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
	BindCombinedImageSamplerToRenderDataTable(14, terrainMaterial._RenderDataTable, terrainMaterial._FifthLayerMaterialProperties, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)));
}

/*
*	Initializes a dynamic physical entity.
*/
void RenderingSystem::InitializeDynamicPhysicalEntity(const Entity *const RESTRICT entity, const DynamicPhysicalInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Cache the components.
	DynamicOutlineRenderComponent &outlineRenderComponent{ ComponentManager::GetDynamicPhysicalDynamicOutlineRenderComponents()[entity->_ComponentsIndex] };
	DynamicPhysicalRenderComponent &physicalRenderComponent{ ComponentManager::GetDynamicPhysicalDynamicPhysicalRenderComponents()[entity->_ComponentsIndex] };
	FrustumCullingComponent &cullingComponent{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[entity->_ComponentsIndex] };
	TransformComponent &transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents()[entity->_ComponentsIndex] };

	//Initialize the outline render component.
	outlineRenderComponent._PhysicalFlags = data->_PhysicalFlags;
	outlineRenderComponent._IsInViewFrustum = true;
	outlineRenderComponent._Buffer = data->_Model._Buffer;
	outlineRenderComponent._IndexOffset = data->_Model._IndexOffset;
	outlineRenderComponent._IndexCount = data->_Model._IndexCount;
	outlineRenderComponent._Color = data->_OutlineColor;

	//Initialize the physical render component.
	physicalRenderComponent._PhysicalFlags = data->_PhysicalFlags;
	physicalRenderComponent._IsInViewFrustum = true;
	physicalRenderComponent._RenderDataTable = data->_Material._RenderDataTable;
	physicalRenderComponent._Buffer = data->_Model._Buffer;
	physicalRenderComponent._IndexOffset = data->_Model._IndexOffset;
	physicalRenderComponent._IndexCount = data->_Model._IndexCount;

	//Initialize the culling component.
	cullingComponent._ModelSpaceAxisAlignedBoundingBox = data->_Model._AxisAlignedBoundingBox;

	//Initialize the transform component.
	transformComponent._Position = data->_Position;
	transformComponent._Rotation = data->_Rotation;
	transformComponent._Scale = data->_Scale;
}

/*
*	Initializes a particle system entity.
*/
void RenderingSystem::InitializeParticleSystemEntity(const Entity *const RESTRICT entity, const ParticleSystemInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Cache the components.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemParticleSystemComponents()[entity->_ComponentsIndex] };
	ParticleSystemRenderComponent &renderComponent{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[entity->_ComponentsIndex] };

	//Fill in the components.
	RenderingUtilities::CalculateAxisAlignedBoundingBoxForParticleSystem(data->_Position, data->_ParticleSystemProperties, &component._AxisAlignedBoundingBox);
	component._Properties = data->_ParticleSystemProperties;
	component._PropertiesUniformBuffer = CreateUniformBuffer(sizeof(ParticleSystemProperties));
	UploadDataToUniformBuffer(component._PropertiesUniformBuffer, &component._Properties);
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem), &renderComponent._RenderDataTable);
	BindUniformBufferToRenderDataTable(0, renderComponent._RenderDataTable, component._PropertiesUniformBuffer);
	BindCombinedImageSamplerToRenderDataTable(1, renderComponent._RenderDataTable, data->_Material._AlbedoTexture, GetSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge)));
	renderComponent._InstanceCount = CatalystBaseMath::Round<uint32>(data->_ParticleSystemProperties._Lifetime / data->_ParticleSystemProperties._SpawnFrequency);
	renderComponent._WorldPosition = data->_Position;
	renderComponent._ParticleSystemRandomSeed = CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f);
	renderComponent._ParticleSystemStartingTime = EngineSystem::Instance->GetTotalTime();
}

/*
*	Terminates a particle system entity.
*/
void RenderingSystem::TerminateParticleSystemEntity(const Entity *const RESTRICT entity) const NOEXCEPT
{
	//Destroy the uniform buffer.
	DestroyUniformBuffer(ComponentManager::GetParticleSystemParticleSystemComponents()[entity->_ComponentsIndex]._PropertiesUniformBuffer);

	//Destroy the render data table.
	DestroyRenderDataTable(ComponentManager::GetParticleSystemParticleSystemRenderComponents()[entity->_ComponentsIndex]._RenderDataTable);
}

/*
*	Finalizes the initialization of a render pass.
*/
void RenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT
{
	//Finalize the initialization of this render pass via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->FinalizeRenderPassInitialization(_RenderPass);
}

/*
*	Initializes all depth buffers.
*/
void RenderingSystem::InitializeDepthBuffers() NOEXCEPT
{
	_DepthBuffers[UNDERLYING(DepthBuffer::DirectionalLight)] = CreateDepthBuffer(GetDirectionalShadowMapResolution());
	_DepthBuffers[UNDERLYING(DepthBuffer::SceneBuffer)] = CreateDepthBuffer(GetScaledResolution());
}

/*
*	Initializes all render targets.
*/
void RenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all render targets.
	CreateRenderTarget(GetDirectionalShadowMapResolution(), TextureFormat::R8_Byte, TextureFilter::Nearest, AddressMode::ClampToBorder, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadowMap)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, TextureFilter::Nearest, AddressMode::ClampToBorder, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadow)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, TextureFilter::Linear, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferAlbedo)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferNormalDepth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferMaterialProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneIntermediate)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Linear, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
}

/*
*	Initializes all samplers.
*/
void RenderingSystem::InitializeSamplers() NOEXCEPT
{
	_Samplers[SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat));
	_Samplers[SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge));
	_Samplers[SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToBorder)] = CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToBorder));
	_Samplers[SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge));
	_Samplers[SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat));
	_Samplers[SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::ClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::ClampToEdge));
}

/*
*	Registers all render passes.
*/
void RenderingSystem::RegisterRenderPasses() NOEXCEPT
{
	//Register all render passes.
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalDynamicPhysicalShadow)] = DirectionalDynamicPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalSolidVegetationShadow)] = DirectionalSolidVegetationShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::TerrainDepth)] = TerrainDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::TerrainColor)] = TerrainColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicPhysical)] = DynamicPhysicalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailSolidVegetation)] = HighDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailSolidVegetation)] = MediumDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailSolidVegetation)] = LowDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailGrassVegetationDepth)] = HighDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailGrassVegetationColor)] = HighDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailGrassVegetationDepth)] = MediumDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailGrassVegetationColor)] = MediumDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailGrassVegetationDepth)] = LowDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailGrassVegetationColor)] = LowDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Lighting)] = LightingRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Sky)] = SkyRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicOutline)] = DynamicOutlineRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::WorldPosition)] = WorldPositionRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
#if defined(CATALYST_ENABLE_OCEAN)
	_RenderPasses[UNDERLYING(RenderPassSubStage::AboveOcean)] = AboveOceanRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BelowOcean)] = BelowOceanRenderPass::Instance.Get();
#endif
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	_RenderPasses[UNDERLYING(RenderPassSubStage::VolumetricFog)] = VolumetricFogRenderPass::Instance.Get();
#endif
#if !defined(CATALYST_FINAL)
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugAxisAlignedBoundingBox)] = DebugAxisAlignedBoundingBoxRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugScreenBox)] = DebugScreenBoxRenderPass::Instance.Get();
#endif
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomHorizontal)] = BloomHorizontalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomVertical)] = BloomVerticalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DepthOfFieldHorizontal)] = DepthOfFieldHorizontalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DepthOfFieldVertical)] = DepthOfFieldVerticalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ToneMapping)] = ToneMappingRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::AntiAliasing)] = AntiAliasingRenderPass::Instance.Get();
}

/*
*	Initializes all render passes.
*/
void RenderingSystem::InitializeRenderPasses() NOEXCEPT
{
	//Initialize all render passes.
	for (RenderPass *const RESTRICT _RenderPass : _RenderPasses)
	{
		_RenderPass->InitializeAsynchronous();
	}

	//Wait for all render passes to finish initialization.
	for (RenderPass *const RESTRICT _RenderPass : _RenderPasses)
	{
		_RenderPass->WaitForInitialization();
	}
}

/*
*	Initializes the common environment materials.
*/
void RenderingSystem::InitializeCommonEnvironmentMaterials() NOEXCEPT
{
	{
		//Create the black common environment material.
		EnvironmentMaterialData data;
		CommonEnvironmentMaterialData::GetBlackEnvironmentMaterialData(&data);
		CreateEnvironmentMaterial(data, _CommonEnvironmentMaterials[UNDERLYING(CommonEnvironmentMaterial::Black)]);
	}

	{
		//Create the day common environment material.
		EnvironmentMaterialData data;
		CommonEnvironmentMaterialData::GetDayEnvironmentMaterialData(&data);
		CreateEnvironmentMaterial(data, _CommonEnvironmentMaterials[UNDERLYING(CommonEnvironmentMaterial::Day)]);
	}

	{
		//Create the night common environment material.
		EnvironmentMaterialData data;
		CommonEnvironmentMaterialData::GetNightEnvironmentMaterialData(&data);
		CreateEnvironmentMaterial(data, _CommonEnvironmentMaterials[UNDERLYING(CommonEnvironmentMaterial::Night)]);
	}
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Initializes the common ocean materials.
*/
void RenderingSystem::InitializeCommonOceanMaterials() NOEXCEPT
{
	{
		//Create the ocean common ocean material.
		OceanMaterialData data;
		CommonOceanMaterialData::GetOceanOceanMaterialData(&data);
		CreateOceanMaterial(data, _CommonOceanMaterials[UNDERLYING(CommonOceanMaterial::Ocean)]);
	}
}
#endif

/*
*	Initializes the common particle materials.
*/
void RenderingSystem::InitializeCommonParticleMaterials() NOEXCEPT
{
	{
		//Create the white circle common particle material.
		ParticleMaterialData data;
		CommonParticleMaterialData::GetWhiteCircleParticleMaterialData(&data);
		CreateParticleMaterial(data, _CommonParticleMaterials[UNDERLYING(CommonParticleMaterial::WhiteCircle)]);
	}
}

/*
*	Initializes the common physical materials.
*/
void RenderingSystem::InitializeCommonPhysicalMaterials() NOEXCEPT
{
	{
		//Create the black common physical material.
		PhysicalMaterialData data;
		CommonPhysicalMaterialData::GetBlackPhysicalMaterialData(&data);
		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[UNDERLYING(CommonPhysicalMaterial::Black)]);
	}

	{
		//Create the red common physical material.
		PhysicalMaterialData data;
		CommonPhysicalMaterialData::GetRedPhysicalMaterialData(&data);
		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[UNDERLYING(CommonPhysicalMaterial::Red)]);
	}

	{
		//Create the teal common physical material.
		PhysicalMaterialData data;
		CommonPhysicalMaterialData::GetTealPhysicalMaterialData(&data);
		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[UNDERLYING(CommonPhysicalMaterial::Teal)]);
	}
}

/*
*	Initializes the common physical models.
*/
void RenderingSystem::InitializeCommonPhysicalModels() NOEXCEPT
{
	{
		//Create the cube common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetCubePhysicalModelData(&data);
		CreatePhysicalModel(data, _CommonPhysicalModels[UNDERLYING(CommonPhysicalModel::Cube)]);
	}

	{
		//Create the octahedron common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetOctahedronPhysicalModelData(&data);
		CreatePhysicalModel(data, _CommonPhysicalModels[UNDERLYING(CommonPhysicalModel::Octahedron)]);
	}

	{
		//Create the plane common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetPlanePhysicalModelData(&data);
		CreatePhysicalModel(data, _CommonPhysicalModels[UNDERLYING(CommonPhysicalModel::Plane)]);
	}
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, ShaderStage::Vertex | ShaderStage::TessellationControl | ShaderStage::TessellationEvaluation | ShaderStage::Geometry | ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::DynamicUniformData)]);
	}

	{
		//Initialize the environment material render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::EnvironmentMaterial)]);
	}

	{
		//Initialize the physical render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 15> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(9, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(10, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(11, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(12, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(13, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(14, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::TerrainMaterial)]);
	}

	{
		//Initialize the physical material render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::PhysicalMaterial)]);
	}

#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Initialize the ocean render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::OceanMaterial)]);
	}
#endif

	{
		//Initialize the particle system render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, ShaderStage::Geometry),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::ParticleSystem)]);
	}

	{
		//Initialize the bloom render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GaussianBlur)]);
	}

	{
		//Initialize the grass material render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GrassMaterial)]);
	}

	{
		//Initialize the terrain render data table layout.
		StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Vertex),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Terrain)]);
	}
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM