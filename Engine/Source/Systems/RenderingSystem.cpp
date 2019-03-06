//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Entities.
#include <Entities/Types/Entity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Engine/CommonEnvironmentMaterialData.h>
#include <Rendering/Engine/CommonOceanMaterialData.h>
#include <Rendering/Engine/CommonParticleMaterialData.h>
#include <Rendering/Engine/CommonPhysicalMaterialData.h>
#include <Rendering/Engine/CommonPhysicalModelData.h>
#include <Rendering/Engine/DynamicUniformData.h>
#include <Rendering/Engine/OceanMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/Engine/TextureData.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/GrassVegetationMaterialData.h>
#include <Resources/GrassVegetationModelData.h>
#include <Resources/OceanMaterialData.h>
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/TreeVegetationMaterialData.h>
#include <Resources/TreeVegetationModelData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TerrainSystem.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>
#include <Vegetation/TreeVegetationMaterial.h>
#include <Vegetation/TreeVegetationModel.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Define the current rendering system.
#if defined(CATALYST_RENDERING_OPENGL)
	#include <Rendering/Translation/OpenGL/OpenGLRenderingSystem.h>
	#define CURRENT_RENDERING_SYSTEM OpenGLRenderingSystem
#elif defined(CATALYST_RENDERING_VULKAN)
	#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>
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

	//Pre-initialize the global render data.
	PreInitializeGlobalRenderData();

	//Initialize the common environment materials.
	InitializeCommonEnvironmentMaterials();

	//Initialize the common ocean materials.
	InitializeCommonOceanMaterials();

	//Initialize the common particle materials.
	InitializeCommonParticleMaterials();

	//Initializes the common physical materials.
	InitializeCommonPhysicalMaterials();

	//Initialize the common physical models.
	InitializeCommonPhysicalModels();

	//Post-initialize the global render data.
	PostInitializeGlobalRenderData();

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

/*
*	Updates the rendering system synchronously.
*/
void RenderingSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Render-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PreUpdateSystemSynchronous();

	//Update the global render data.
	UpdateGlobalRenderData();

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
*	Returns the number of frame buffers
*/
uint8 RenderingSystem::GetNumberOfFrameBuffers() const NOEXCEPT
{
	//Return the number of frame buffers via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetNumberOfFrameBuffers();
}

/*
*	Returns the current frame buffer index.
*/
uint8 RenderingSystem::GetCurrentFrameBufferIndex() const NOEXCEPT
{
	//Return the current frame buffer index via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentFrameBufferIndex();
}

/*
*	Creates a buffer.
*/
ConstantBufferHandle RenderingSystem::CreateBuffer(const uint64 size) const NOEXCEPT
{
	//Create the buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateBuffer(size);
}

/*
*	Uploads data to a buffer.
*/
void RenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, ConstantBufferHandle handle) const NOEXCEPT
{
	//Upload the data to the buffer via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->UploadDataToBuffer(data, dataSizes, dataChunks, handle);
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
void RenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	//Bind the combined image sampler to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindCombinedImageSamplerToRenderDataTable(binding, arrayElement, renderDataTable, image, sampler);
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image) const NOEXCEPT
{
	//Bind the sampled image to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindSampledImageToRenderDataTable(binding, arrayElement, renderDataTable, image);
}

/*
*	Binds a sampler to a render data table.
*/
void RenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, SamplerHandle sampler) const NOEXCEPT
{
	//Bind the sampler to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindSamplerToRenderDataTable(binding, arrayElement, renderDataTable, sampler);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void RenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT
{
	//Bind the uniform buffer to the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->BindUniformBufferToRenderDataTable(binding, arrayElement, renderDataTable, uniformBuffer);
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
void RenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateRenderTarget(resolution, format, handle);
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
UniformBufferHandle RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize, const BufferUsage usage) const NOEXCEPT
{
	//Create the uniform buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateUniformBuffer(uniformBufferSize, usage);
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
*	Returns the global render data table.
*/
RenderDataTableHandle RenderingSystem::GetGlobalRenderDataTable() const NOEXCEPT
{
	return _GlobalRenderData._RenderDataTables[GetCurrentFrameBufferIndex()];
}

/*
*	Adds a terrain height texture to the global render data and returns it's index.
*/
uint8 RenderingSystem::AddTerrainHeightTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT
{
	//Lock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	//Find the first available index and store it.
	uint8 index{ UINT8_MAXIMUM };

	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES; ++i)
	{
		//If this is available, grab it!
		if (!_GlobalRenderData._TerrainHeightTextureSlots[i])
		{
			index = i;
			_GlobalRenderData._TerrainHeightTextureSlots[i] = true;

			break;
		}
	}

	ASSERT(index != UINT8_MAXIMUM, "If no index could be found, then, well... This is bad. ):");

	//Add the terrain height texture updates.
	for (DynamicArray<Pair<uint8, Texture2DHandle>> &terrainHeightTextureUpdate : _GlobalRenderData._AddTerrainHeightTextureUpdates)
	{
		terrainHeightTextureUpdate.EmplaceSlow(index, texture);
	}

	//Unlock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();

	//Return the index.
	return index;
}

/*
*	Returns a terrain height texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTerrainHeightTextureToGlobalRenderData(const uint8 index) NOEXCEPT
{
	//Lock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	//Add the terrain height texture updates.
	for (DynamicArray<uint8> &terrainHeightTextureUpdate : _GlobalRenderData._RemoveTerrainHeightTextureUpdates)
	{
		terrainHeightTextureUpdate.EmplaceSlow(index);
	}

	//Mark the terrain height texture slot as available.
	_GlobalRenderData._TerrainHeightTextureSlots[index] = false;

	//Unlock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();
}

/*
*	Adds a texture to the global render data and returns it's index.
*/
uint32 RenderingSystem::AddTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT
{
	//Lock global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	//Find the first available index and store it.
	uint32 index{ UINT32_MAXIMUM };

	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		//If this is available, grab it!
		if (!_GlobalRenderData._GlobalTextureSlots[i])
		{
			index = i;
			_GlobalRenderData._GlobalTextureSlots[i] = true;

			break;
		}
	}

	ASSERT(index != UINT32_MAXIMUM, "If no index could be found, then, well... This is bad. ):");

	//Add the global texture updates.
	for (DynamicArray<Pair<uint32, Texture2DHandle>> &globalTextureUpdate : _GlobalRenderData._AddGlobalTextureUpdates)
	{
		globalTextureUpdate.EmplaceSlow(index, texture);
	}

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();

	//Return the index.
	return index;
}

/*
*	Returns a texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTextureToGlobalRenderData(const uint32 index) NOEXCEPT
{
	//Lock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	//Add the global texture updates.
	for (DynamicArray<uint32> &globalTextureUpdate : _GlobalRenderData._RemoveGlobalTextureUpdates)
	{
		globalTextureUpdate.EmplaceSlow(index);
	}

	//Mark the global texture slot as available.
	_GlobalRenderData._GlobalTextureSlots[index] = false;

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
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
	BindCombinedImageSamplerToRenderDataTable(0, 0, material._RenderDataTable, material._DiffuseTexture, GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
	BindCombinedImageSamplerToRenderDataTable(1, 0, material._RenderDataTable, material._DiffuseIrradianceTexture, GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
}

/*
*	Creates a grass vegetation material.
*/
void RenderingSystem::CreateGrassVegetationMaterial(const GrassVegetationMaterialData &data, GrassVegetationMaterial &material) NOEXCEPT
{
	//Create the mask texture.
	material._MaskTexture = CreateTexture2D(TextureData(TextureDataContainer(data._MaskData, data._MaskWidth, data._MaskHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the albedo texture.
	material._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(data._AlbedoData, data._Width, data._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	material._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(data._NormalMapData, data._Width, data._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial), &material._RenderDataTable);
	BindCombinedImageSamplerToRenderDataTable(0, 0, material._RenderDataTable, material._MaskTexture, GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	BindCombinedImageSamplerToRenderDataTable(1, 0, material._RenderDataTable, material._AlbedoTexture, GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	BindCombinedImageSamplerToRenderDataTable(2, 0, material._RenderDataTable, material._NormalMapTexture, GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
}

/*
*	Creates a grass vegetation model.
*/
void RenderingSystem::CreateGrassVegetationModel(const GrassVegetationModelData &data, GrassVegetationModel &model) NOEXCEPT
{
	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ data._Vertices.Data(), data._Indices.Data() };
	const uint64 modelDataSizes[]{ sizeof(VegetationVertex) * data._Vertices.Size(), sizeof(uint32) * data._Indices.Size() };
	ConstantBufferHandle buffer = CreateBuffer(modelDataSizes[0] + modelDataSizes[1]);
	UploadDataToBuffer(modelData, modelDataSizes, 2, buffer);

	//Set up the grass vegetation model.
	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		model._AxisAlignedBoundingBoxes[i]._Minimum = Vector3<float>(-data._Extent * 0.5f, -data._Extent * 0.5f, -data._Extent * 0.5f);
		model._AxisAlignedBoundingBoxes[i]._Maximum = Vector3<float>(data._Extent * 0.5f, data._Extent * 0.5f, data._Extent * 0.5f);
		model._Buffers[i] = buffer;
		model._IndexOffsets[i] = modelDataSizes[0];
		model._IndexCounts[i] = static_cast<uint32>(data._Indices.Size());
	}
}

/*
*	Creates an ocean material.
*/
void RenderingSystem::CreateOceanMaterial(const OceanMaterialData &data, OceanMaterial &material) const NOEXCEPT
{
	//Create the normal texture.
	material._NormalTexture = CreateTexture2D(TextureData(TextureDataContainer(data._NormalData, data._Width, data._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the foam texture.
	material._FoamTexture = CreateTexture2D(TextureData(TextureDataContainer(data._FoamData, data._Width, data._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::OceanMaterial), &material._RenderDataTable);

	//Update the render data table.
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, material._RenderDataTable, material._NormalTexture, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, material._RenderDataTable, material._FoamTexture, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat));
}

/*
*	Creates a physical model.
*/
void RenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Create the vertex and index buffer.
		const void *RESTRICT modelData[]{ physicalModelData._Vertices[i].Data(), physicalModelData._Indices[i].Data() };
		const uint64 modelDataSizes[]{ sizeof(PhysicalVertex) * physicalModelData._Vertices[i].Size(), sizeof(uint32) * physicalModelData._Indices[i].Size() };
		ConstantBufferHandle buffer = CreateBuffer(modelDataSizes[0] + modelDataSizes[1]);
		UploadDataToBuffer(modelData, modelDataSizes, 2, buffer);

		//Set up the physical model.
		physicalModel._AxisAlignedBoundingBoxes[i]._Minimum = Vector3<float>(-physicalModelData._Extents[i] * 0.5f, -physicalModelData._Extents[i] * 0.5f, -physicalModelData._Extents[i] * 0.5f);
		physicalModel._AxisAlignedBoundingBoxes[i]._Maximum = Vector3<float>(physicalModelData._Extents[i] * 0.5f, physicalModelData._Extents[i] * 0.5f, physicalModelData._Extents[i] * 0.5f);
		physicalModel._Buffers[i] = buffer;
		physicalModel._IndexOffsets[i] = modelDataSizes[0];
		physicalModel._IndexCounts[i] = static_cast<uint32>(physicalModelData._Indices[i].Size());
	}
}

/*
*	Creates a particle material.
*/
void RenderingSystem::CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT
{
	//Create the albedo texture
	particleMaterial._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(particleMaterialData._AlbedoData, particleMaterialData._Width, particleMaterialData._Height, 4), TextureFormat::R8G8B8A8_Byte));
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) NOEXCEPT
{
	//Create the albedo texture.
	physicalMaterial._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._AlbedoData, physicalMaterialData._Width, physicalMaterialData._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	physicalMaterial._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._NormalMapData, physicalMaterialData._Width, physicalMaterialData._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the material properties texture.
	physicalMaterial._MaterialPropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(physicalMaterialData._MaterialPropertiesData, physicalMaterialData._Width, physicalMaterialData._Height, 4), TextureFormat::R8G8B8A8_Byte));

	//Add the textures to the global render data table.
	physicalMaterial._AlbedoTextureIndex = AddTextureToGlobalRenderData(physicalMaterial._AlbedoTexture);
	physicalMaterial._NormalMapTextureIndex = AddTextureToGlobalRenderData(physicalMaterial._NormalMapTexture);
	physicalMaterial._MaterialPropertiesTextureIndex = AddTextureToGlobalRenderData(physicalMaterial._MaterialPropertiesTexture);
}

/*
*	Creates a tree vegetation material.
*/
void RenderingSystem::CreateTreeVegetationMaterial(const TreeVegetationMaterialData &data, TreeVegetationMaterial &material) NOEXCEPT
{
	//Create the crown mask texture.
	material._CrownMaskTexture = CreateTexture2D(TextureData(TextureDataContainer(data._CrownMaskData, data._CrownMaskWidth, data._CrownMaskHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the crown albedo texture.
	material._CrownAlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(data._CrownAlbedoData, data._CrownWidth, data._CrownHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the crown map texture.
	material._CrownNormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(data._CrownNormalMapData, data._CrownWidth, data._CrownHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the impostor mask texture.
	material._ImpostorMaskTexture = CreateTexture2D(TextureData(TextureDataContainer(data._ImpostorMaskData, data._ImpostorMaskWidth, data._ImpostorMaskHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the impostor albedo texture.
	material._ImpostorAlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(data._ImpostorAlbedoData, data._ImpostorWidth, data._ImpostorHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the trunk albedo texture.
	material._TrunkAlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(data._TrunkAlbedoData, data._TrunkWidth, data._TrunkHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	material._TrunkNormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(data._TrunkNormalMapData, data._TrunkWidth, data._TrunkHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Create the material properties texture.
	material._TrunkMaterialPropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(data._TrunkMaterialPropertiesData, data._TrunkWidth, data._TrunkHeight, 4), TextureFormat::R8G8B8A8_Byte));

	//Add the textures to the global render data table.
	material._CrownMaskTextureIndex = AddTextureToGlobalRenderData(material._CrownMaskTexture);
	material._CrownAlbedoTextureIndex = AddTextureToGlobalRenderData(material._CrownAlbedoTexture);
	material._CrownNormalMapTextureIndex = AddTextureToGlobalRenderData(material._CrownNormalMapTexture);
	material._ImpostorMaskTextureIndex = AddTextureToGlobalRenderData(material._ImpostorMaskTexture);
	material._ImpostorAlbedoTextureIndex = AddTextureToGlobalRenderData(material._ImpostorAlbedoTexture);
	material._TrunkAlbedoTextureIndex = AddTextureToGlobalRenderData(material._TrunkAlbedoTexture);
	material._TrunkNormalMapTextureIndex = AddTextureToGlobalRenderData(material._TrunkNormalMapTexture);
	material._TrunkMaterialPropertiesTextureIndex = AddTextureToGlobalRenderData(material._TrunkMaterialPropertiesTexture);
}

/*
*	Creates a tree vegetation model.
*/
void RenderingSystem::CreateTreeVegetationModel(const TreeVegetationModelData &data, TreeVegetationModel &model) NOEXCEPT
{
	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Create the vertex and index buffer.
		const void *RESTRICT modelData[]{ data._CrownVertices[i].Data(), data._CrownIndices[i].Data() };
		const uint64 modelDataSizes[]{ sizeof(VegetationVertex) * data._CrownVertices[i].Size(), sizeof(uint32) * data._CrownIndices[i].Size() };
		ConstantBufferHandle buffer = CreateBuffer(modelDataSizes[0] + modelDataSizes[1]);
		UploadDataToBuffer(modelData, modelDataSizes, 2, buffer);

		//Set up the model.
		model._CrownBuffers[i] = buffer;
		model._CrownIndexOffsets[i] = modelDataSizes[0];
		model._CrownIndexCounts[i] = static_cast<uint32>(data._CrownIndices[i].Size());
	}

	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Create the vertex and index buffer.
		const void *RESTRICT modelData[]{ data._TrunkVertices[i].Data(), data._TrunkIndices[i].Data() };
		const uint64 modelDataSizes[]{ sizeof(VegetationVertex) * data._TrunkVertices[i].Size(), sizeof(uint32) * data._TrunkIndices[i].Size() };
		ConstantBufferHandle buffer = CreateBuffer(modelDataSizes[0] + modelDataSizes[1]);
		UploadDataToBuffer(modelData, modelDataSizes, 2, buffer);

		//Set up the model.
		model._AxisAlignedBoundingBoxes[i]._Minimum = Vector3<float>(-data._TrunkExtents[i] * 0.5f, -data._TrunkExtents[i] * 0.5f, -data._TrunkExtents[i] * 0.5f);
		model._AxisAlignedBoundingBoxes[i]._Maximum = Vector3<float>(data._TrunkExtents[i] * 0.5f, data._TrunkExtents[i] * 0.5f, data._TrunkExtents[i] * 0.5f);
		model._TrunkBuffers[i] = buffer;
		model._TrunkIndexOffsets[i] = modelDataSizes[0];
		model._TrunkIndexCounts[i] = static_cast<uint32>(data._TrunkIndices[i].Size());
	}
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
*	Pre-initializes the global render data.
*/
void RenderingSystem::PreInitializeGlobalRenderData() NOEXCEPT
{
	//Get the number of frame buffers.
	const uint8 numberOfFrameBuffers{ GetNumberOfFrameBuffers() };

	//Upsize the buffers.
	_GlobalRenderData._RenderDataTables.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._DynamicUniformDataBuffers.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._RemoveGlobalTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._AddGlobalTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._RemoveTerrainHeightTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._AddTerrainHeightTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._TerrainPatchDataBuffers.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._TerrainMaterialDataBuffers.UpsizeFast(numberOfFrameBuffers);

	for (uint8 i{ 0 }; i < numberOfFrameBuffers; ++i)
	{
		//Create the render data table.
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global), &_GlobalRenderData._RenderDataTables[i]);

		//Create the dynamic uniform data buffer.
		_GlobalRenderData._DynamicUniformDataBuffers[i] = CreateUniformBuffer(sizeof(DynamicUniformData), BufferUsage::UniformBuffer);

		//Bind the dynamic uniform data buffer to the render data table.
		BindUniformBufferToRenderDataTable(0, 0, _GlobalRenderData._RenderDataTables[i], _GlobalRenderData._DynamicUniformDataBuffers[i]);

		//Bind all samplers.
		for (uint8 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(1, j, _GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Create the terrain patch data buffer.
		_GlobalRenderData._TerrainPatchDataBuffers[i] = CreateUniformBuffer(sizeof(TerrainPatchInstanceRenderInformation) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, BufferUsage::UniformBuffer);
	
		//Bind the terrain patch data buffer to the render data table.
		BindUniformBufferToRenderDataTable(4, 0, _GlobalRenderData._RenderDataTables[i], _GlobalRenderData._TerrainPatchDataBuffers[i]);

		//Create the terrain material data buffer.
		_GlobalRenderData._TerrainMaterialDataBuffers[i] = CreateUniformBuffer(sizeof(TerrainMaterial) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, BufferUsage::UniformBuffer);

		//Bind the terrain material data buffer to the render data table.
		BindUniformBufferToRenderDataTable(5, 0, _GlobalRenderData._RenderDataTables[i], _GlobalRenderData._TerrainMaterialDataBuffers[i]);
	}

	//Mark all global texture slots as free.
	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		_GlobalRenderData._GlobalTextureSlots[i] = false;
	}

	//Mark all terrain height texture slots as free.
	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES; ++i)
	{
		_GlobalRenderData._TerrainHeightTextureSlots[i] = false;
	}
}

/*
*	Post-initializes the global render data.
*/
void RenderingSystem::PostInitializeGlobalRenderData() NOEXCEPT
{
	//Get the number of frame buffers.
	const uint8 numberOfFrameBuffers{ GetNumberOfFrameBuffers() };

	for (uint8 i{ 0 }; i < numberOfFrameBuffers; ++i)
	{
		//Bind a placeholder texture to all global texture slots.
		for (uint32 j{ 0 }; j < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++j)
		{
			BindSampledImageToRenderDataTable(2, j, _GlobalRenderData._RenderDataTables[i], GetCommonPhysicalMaterial(CommonPhysicalMaterial::Black)._AlbedoTexture);
		}

		//Bind a placeholder texture to all terrain height texture slots.
		for (uint8 j{ 0 }; j < RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES; ++j)
		{
			BindCombinedImageSamplerToRenderDataTable(3, j, _GlobalRenderData._RenderDataTables[i], GetCommonPhysicalMaterial(CommonPhysicalMaterial::Black)._AlbedoTexture, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
		}
	}
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
	CreateRenderTarget(GetDirectionalShadowMapResolution(), TextureFormat::R32_Float, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadowMap)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadow)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate)]);
	CreateRenderTarget(GetScaledResolution() / 2, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateHalf)]);
	CreateRenderTarget(GetScaledResolution() / 4, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateQuarter)]);
	CreateRenderTarget(GetScaledResolution() / 8, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateEighth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferAlbedo)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferNormalDepth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferMaterialProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::ScreenSpaceAmbientOcclusion)]);
}

/*
*	Initializes all samplers.
*/
void RenderingSystem::InitializeSamplers() NOEXCEPT
{
	_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge));
	_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat));
	_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::ClampToEdge));
	_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat)] = CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat));
	_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToBorder)] = CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToBorder));
	_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)] = CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge));
}

/*
*	Registers all render passes.
*/
void RenderingSystem::RegisterRenderPasses() NOEXCEPT
{
	//Register all render passes.
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalDynamicPhysicalShadow)] = DirectionalDynamicPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalTreeVegetationTrunkShadow)] = DirectionalTreeVegetationTrunkRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalTreeVegetationCrownShadow)] = DirectionalTreeVegetationCrownShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalSolidVegetationShadow)] = DirectionalSolidVegetationShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::TerrainDepth)] = TerrainDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::TerrainColor)] = TerrainColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicPhysical)] = DynamicPhysicalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailTreeVegetationTrunk)] = HighDetailTreeVegetationTrunkRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailTreeVegetationTrunk)] = MediumDetailTreeVegetationTrunkRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailTreeVegetationTrunk)] = LowDetailTreeVegetationTrunkRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailTreeVegetationCrownDepth)] = HighDetailTreeVegetationCrownDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailTreeVegetationCrownColor)] = HighDetailTreeVegetationCrownColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailTreeVegetationCrownDepth)] = MediumDetailTreeVegetationCrownDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailTreeVegetationCrownColor)] = MediumDetailTreeVegetationCrownColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailTreeVegetationCrownDepth)] = LowDetailTreeVegetationCrownDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailTreeVegetationCrownColor)] = LowDetailTreeVegetationCrownColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailTreeVegetationImpostorDepth)] = HighDetailTreeVegetationImpostorDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailTreeVegetationImpostorColor)] = HighDetailTreeVegetationImpostorColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailTreeVegetationImpostorDepth)] = LowDetailTreeVegetationImpostorDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailTreeVegetationImpostorColor)] = LowDetailTreeVegetationImpostorColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailSolidVegetation)] = HighDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailSolidVegetation)] = MediumDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailSolidVegetation)] = LowDetailSolidVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailDebrisVegetation)] = HighDetailDebrisVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailDebrisVegetation)] = MediumDetailDebrisVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailDebrisVegetation)] = LowDetailDebrisVegetationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailGrassVegetationDepth)] = HighDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::HighDetailGrassVegetationColor)] = HighDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailGrassVegetationDepth)] = MediumDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::MediumDetailGrassVegetationColor)] = MediumDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailGrassVegetationDepth)] = LowDetailGrassVegetationDepthRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::LowDetailGrassVegetationColor)] = LowDetailGrassVegetationColorRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalShadowHorizontalBlur)] = DirectionalShadowHorizontalBlurRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalShadowVerticalBlur)] = DirectionalShadowVerticalBlurRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ScreenSpaceAmbientOcclusion)] = ScreenSpaceAmbientOcclusionRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ScreenSpaceAmbientOcclusionHorizontalBlur)] = ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ScreenSpaceAmbientOcclusionVerticalBlur)] = ScreenSpaceAmbientOcclusionVerticalBlurRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Lighting)] = LightingRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::PointLight)] = PointLightRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Sky)] = SkyRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicOutline)] = DynamicOutlineRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::AboveOcean)] = AboveOceanRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BelowOcean)] = BelowOceanRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ExponentialHeightFog)] = ExponentialHeightFogRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DepthOfFieldHorizontal)] = DepthOfFieldHorizontalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DepthOfFieldVertical)] = DepthOfFieldVerticalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::VolumetricFog)] = VolumetricFogRenderPass::Instance.Get();
#if defined(CATALYST_CONFIGURATION_DEBUG)
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugAxisAlignedBoundingBox)] = DebugAxisAlignedBoundingBoxRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugScreenBox)] = DebugScreenBoxRenderPass::Instance.Get();
#endif
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomDownsampleFirstIteration)] = BloomDownsampleFirstIterationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomDownsampleSecondIteration)] = BloomDownsampleSecondIterationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomDownsampleThirdIteration)] = BloomDownsampleThirdIterationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomUpsampleFirstIteration)] = BloomUpsampleFirstIterationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomUpsampleSecondIteration)] = BloomUpsampleSecondIterationRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::BloomUpsampleThirdIteration)] = BloomUpsampleThirdIterationRenderPass::Instance.Get();
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

	//Set the night/day environment materials to default ones.
	EnvironmentManager::Instance->SetNightEnvironmentMaterial(_CommonEnvironmentMaterials[UNDERLYING(CommonEnvironmentMaterial::Night)]);
	EnvironmentManager::Instance->SetDayEnvironmentMaterial(_CommonEnvironmentMaterials[UNDERLYING(CommonEnvironmentMaterial::Day)]);
	EnvironmentManager::Instance->SetEnvironmentBlend(1.0f);
}

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

	//Set the ocean material to default one.
	EnvironmentManager::Instance->SetOceanMaterial(_CommonOceanMaterials[UNDERLYING(CommonOceanMaterial::Ocean)]);
}

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

	{
		//Create the sphere common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetSpherePhysicalModelData(&data);
		CreatePhysicalModel(data, _CommonPhysicalModels[UNDERLYING(CommonPhysicalModel::Sphere)]);
	}
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 6> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Vertex | ShaderStage::TessellationControl | ShaderStage::TessellationEvaluation | ShaderStage::Geometry | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::SampledImage, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, ShaderStage::Vertex),
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
	}

	{
		//Initialize the environment material render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::EnvironmentMaterial)]);
	}

	{
		//Initialize the ocean render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::OceanMaterial)]);
	}

	{
		//Initialize the particle system render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::ParticleSystem)]);
	}

	{
		//Initialize the bloom render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GaussianBlur)]);
	}

	{
		//Initialize the grass material render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GrassMaterial)]);
	}
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalRenderData() NOEXCEPT
{
	//Retrieve the current frame buffer index.
	const uint8 currentFrameBufferIndex{ GetCurrentFrameBufferIndex() };

	//Update the dynamic uniform data.
	UpdateDynamicUniformData(currentFrameBufferIndex);

	//Update the global textures.
	UpdateGlobalTextures(currentFrameBufferIndex);

	//Update the terrain height textures.
	UpdateTerrainHeightTextures(currentFrameBufferIndex);

	//Update the terrain patch data.
	UpdateTerrainPatchData(currentFrameBufferIndex);

	//Update the terrain material data.
	UpdateTerrainMaterialData(currentFrameBufferIndex);
}

/*
*	Updates the dynamic uniform data.
*/
void RenderingSystem::UpdateDynamicUniformData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	DynamicUniformData data;

	//Update matrices.
	data._DirectionalLightViewMatrix = *LightingSystem::Instance->GetDirectionalLight()->GetViewMatrix();
	data._InversePerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	data._InverseProjectionMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	data._PerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	data._ProjectionMatrix = *Perceiver::Instance->GetProjectionMatrix();
	data._ViewMatrix = *Perceiver::Instance->GetViewMatrix();

	//Update vectors.
	data._DirectionalLightColor = LightingSystem::Instance->GetDirectionalLight()->GetColor();
	data._DirectionalLightDirection = LightingSystem::Instance->GetDirectionalLight()->GetDirection();
	data._PerceiverWorldPosition = Perceiver::Instance->GetPosition();
	data._WindDirection = PhysicsSystem::Instance->GetWindDirection();

	//Update floats.
	data._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	data._DirectionalLightIntensity = LightingSystem::Instance->GetDirectionalLight()->GetIntensity();
	data._EnvironmentBlend = EnvironmentManager::Instance->GetEnvironmentBlend();
	data._TotalTime = CatalystEngineSystem::Instance->GetTotalTime();
	data._WindSpeed = PhysicsSystem::Instance->GetWindSpeed();

	UploadDataToUniformBuffer(_GlobalRenderData._DynamicUniformDataBuffers[currentFrameBufferIndex], &data);
}

/*
*	Updates the global textures.
*/
void RenderingSystem::UpdateGlobalTextures(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	//Lock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	//Process all updates.
	for (uint32 update : _GlobalRenderData._RemoveGlobalTextureUpdates[currentFrameBufferIndex])
	{
		BindSampledImageToRenderDataTable(2, update, _GlobalRenderData._RenderDataTables[currentFrameBufferIndex], GetCommonPhysicalMaterial(CommonPhysicalMaterial::Black)._AlbedoTexture);
	}

	_GlobalRenderData._RemoveGlobalTextureUpdates[currentFrameBufferIndex].ClearFast();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[currentFrameBufferIndex])
	{
		BindSampledImageToRenderDataTable(2, update._First, _GlobalRenderData._RenderDataTables[currentFrameBufferIndex], update._Second);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[currentFrameBufferIndex].ClearFast();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
}

/*
*	Updates the terrain height textures.
*/
void RenderingSystem::UpdateTerrainHeightTextures(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	//Lock the terrain height textures.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	//Process all updates.
	for (uint8 update : _GlobalRenderData._RemoveTerrainHeightTextureUpdates[currentFrameBufferIndex])
	{
		BindCombinedImageSamplerToRenderDataTable(3, update, _GlobalRenderData._RenderDataTables[currentFrameBufferIndex], GetCommonPhysicalMaterial(CommonPhysicalMaterial::Black)._AlbedoTexture, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	}

	_GlobalRenderData._RemoveTerrainHeightTextureUpdates[currentFrameBufferIndex].ClearFast();

	for (Pair<uint8, Texture2DHandle> &update : _GlobalRenderData._AddTerrainHeightTextureUpdates[currentFrameBufferIndex])
	{
		BindCombinedImageSamplerToRenderDataTable(3, static_cast<uint32>(update._First), _GlobalRenderData._RenderDataTables[currentFrameBufferIndex], update._Second, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	}

	_GlobalRenderData._AddTerrainHeightTextureUpdates[currentFrameBufferIndex].ClearFast();

	//Unlock the terrain height textures.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();
}

/*
*	Updates the terrain patch data.
*/
void RenderingSystem::UpdateTerrainPatchData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	StaticArray<TerrainPatchInstanceRenderInformation, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES> terrainUniformData;

	const DynamicArray<TerrainPatchRenderInformation> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	uint64 counter{ 0 };

	for (uint64 i{ 0 }, size{ informations->Size() }; i < size; ++i)
	{
		if (!TEST_BIT(informations->At(i)._Visibility, VisibilityFlag::Perceiver))
		{
			continue;
		}

		terrainUniformData[counter++] = informations->At(i)._InstanceInformation;
	}

	UploadDataToUniformBuffer(_GlobalRenderData._TerrainPatchDataBuffers[currentFrameBufferIndex], terrainUniformData.Data());
}

/*
*	Updates the terrain material data.
*/
void RenderingSystem::UpdateTerrainMaterialData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	UploadDataToUniformBuffer(_GlobalRenderData._TerrainMaterialDataBuffers[currentFrameBufferIndex], TerrainSystem::Instance->GetTerrainMaterials()->Data());
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM