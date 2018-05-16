#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>


//Math.
#include <Math/Matrix4.h>

//Multithreading.
#include <Multithreading/MultithreadedResource.h>
#include <Multithreading/Semaphore.h>

//Rendering.
#include <Rendering/Engine Layer/EnvironmentMaterial.h>
#include <Rendering/Engine Layer/PostProcessingUniformData.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/Window.h>
#include <Rendering/Translation Layer/Vulkan/VulkanDynamicUniformData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanFrameData.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class EnvironmentMaterial;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class OceanMaterial;
class ParticleMaterial;
class ParticleMaterialData;
class ParticleSystemEntity;
class ParticleSystemProperties;
class PhysicalMaterial;
class PhysicalMaterialData;
class PhysicalModel;
class PhysicalModelData;
class RenderPass;
class StaticPhysicalEntity;
class TerrainEntity;
class TerrainMaterial;
class TerrainMaterialData;
class TerrainUniformData;
class TextureData;
class VegetationEntity;
class VegetationMaterial;
class VegetationMaterialData;
class VegetationProperties;
class VegetationTransformation;
class WaterEntity;
class WaterMaterialData;
class WaterUniformData;

class VulkanRenderingSystem final
{

public:

	//Singletone declaration.
	DECLARE_SINGLETON(VulkanRenderingSystem);

	/*
	*	Initializes the Vulkan rendering system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the rendering Vulkan system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the Vulkan rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

	/*
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT;

	/*
	*	Creates a terrain material.
	*/
	void CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT;

	/*
	*	Creates and returns physical model.
	*/
	void CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT;

	/*
	*	Creates a particle material.
	*/
	void CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT;

	/*
	*	Creates a physical material.
	*/
	void CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT;

	/*
	*	Creates a vegetation material.
	*/
	void CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT;

	/*
	*	Creates a water material.
	*/
	void CreateWaterMaterial(const WaterMaterialData &waterMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture2D &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT;

	/*
	*	Initializes a static physical entity.
	*/
	void InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Initializes a vegetation entity.
	*/
	void InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT;

	/*
	*	Initializes a particle system entity.
	*/
	void InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT;

	/*
	*	Sets the post processing blur amount.
	*/
	void SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT;

	/*
	*	Sets the post processing chromatic aberration amount.
	*/
	void SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT;

	/*
	*	Sets the post processing saturation.
	*/
	void SetPostProcessingSaturation(const float newSaturation) NOEXCEPT;

	/*
	*	Sets the post processing sharpen amount.
	*/
	void SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT;

private:

	friend class RenderingSystem;

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	DescriptorSetHandle GetCurrentDynamicUniformDataDescriptorSet() NOEXCEPT;

	//Enumeration covering all default textures.
	enum DefaultTexture : uint8
	{
		Black,
		White,
		NumberOfDefaultTextures
	};

	//Enumeration covering all depth buffers.
	enum class DepthBuffer : uint8
	{
		DirectionalLight,
		SceneBuffer,
		NumberOfDepthBuffers
	};

	//Enumeration covering all descriptor sets.
	enum class DescriptorSet : uint8
	{
		Environment,
		ShadowMapBlur,
		Lighting,
		PostProcessing,
		NumberOfDescriptorSets
	};

	//Enumeration covering all descriptor set layouts.
	enum class DescriptorSetLayout : uint8
	{
		DynamicUniformData,
		ShadowMapBlur,
		Environment,
		Terrain,
		Physical,
		Vegetation,
		Lighting,
		Ocean,
		ParticleSystem,
		PostProcessing,
		NumberOfDescriptorSetLayouts
	};

	//Enumeration covering all pipelines.
	enum class Pipeline : uint8
	{
		DirectionalShadowTerrain,
		DirectionalShadowInstancedPhysical,
		ShadowMapBlur,
		Terrain,
		StaticPhysical,
		InstancedPhysical,
		Vegetation,
		Lighting,
		CubeMap,
		Ocean,
		ParticleSystem,
		PostProcessing,
		NumberOfPipelines
	};

	//Enumeration covering all render targets.
	enum class RenderTarget : uint8
	{
		DirectionalPreBlurShadowMap,
		DirectionalPostBlurShadowMap,
		SceneBufferAlbedoColor,
		SceneBufferNormalDirectionDepth,
		SceneBufferRoughnessMetallicAmbientOcclusion,
		Scene,
		WaterScene,
		NumberOfRenderTargets
	};

	//Enumeration covering all semaphores.
	enum class GraphicsSemaphore : uint8
	{
		ImageAvailable,
		RenderFinished,
		NumberOfSemaphores
	};

	//Enumeration covering all shader modules.
	enum class ShaderModule : uint8
	{
		CubeMapFragmentShader,
		CubeMapVertexShader,
		DirectionalShadowInstancedPhysicalVertexShader,
		DirectionalShadowTerrainTessellationEvaluationShader,
		InstancedPhysicalVertexShader,
		LightingFragmentShader,
		OceanFragmentShader,
		ParticleSystemFragmentShader,
		ParticleSystemGeometryShader,
		ParticleSystemVertexShader,
		PostProcessingFragmentShader,
		PhysicalFragmentShader,
		PhysicalVertexShader,
		ShadowMapBlurFragmentShader,
		ShadowMapFragmentShader,
		TerrainFragmentShader,
		TerrainTessellationControlShader,
		TerrainTessellationEvaluationShader,
		TerrainVertexShader,
		VegetationFragmentShader,
		VegetationGeometryShader,
		VegetationVertexShader,
		ViewportVertexShader,
		NumberOfShaderModules
	};

	//Enumeration covering all task semaphores.
	enum class TaskSemaphore : uint8
	{
		RenderDirectionalShadows,
		RenderTerrain,
		RenderStaticPhysicalEntities,
		RenderInstancedPhysicalEntities,
		RenderVegetationEntities,
		RenderLighting,
		RenderSkybox,
		RenderParticleSystemEntities,
		RenderOcean,
		RenderPostProcessing,
		UpdateParticleSystemProperties,
		UpdateVegetationCulling,
		UpdateViewFrustumCuling,
		NumberOfTaskSemaphores
	};

	//Enumeration covering all uniform buffers.
	enum UniformBuffer : uint8
	{
		PostProcessingUniformDataBuffer,
		NumberOfUniformBuffers
	};

	//Container for all task semaphores.
	StaticArray<Semaphore, INDEX(TaskSemaphore::NumberOfTaskSemaphores)> taskSemaphores;

	//The main window.
	Window mainWindow;

	//The active camera.
	CameraEntity *RESTRICT activeCamera;

	//The current projection matrix.
	Matrix4 projectionMatrix;

	//The dynamic uniform data.
	VulkanDynamicUniformData dynamicUniformData;

	//The post processing uniform data.
	PostProcessingUniformData postProcessingUniformData;

	//Container for all default textures.
	StaticArray<Vulkan2DTexture *RESTRICT, DefaultTexture::NumberOfDefaultTextures> defaultTextures;

	//Container for all depth buffers.
	StaticArray<VulkanDepthBuffer *RESTRICT, INDEX(DepthBuffer::NumberOfDepthBuffers)> depthBuffers;

	//Container for all descriptor sets.
	StaticArray<VulkanDescriptorSet, INDEX(DescriptorSet::NumberOfDescriptorSets)> descriptorSets;

	//Container for all descriptor set layouts.
	StaticArray<VulkanDescriptorSetLayout, INDEX(DescriptorSetLayout::NumberOfDescriptorSetLayouts)> descriptorSetLayouts;

	//Container for all render targets.
	StaticArray<VulkanRenderTarget *RESTRICT, INDEX(RenderTarget::NumberOfRenderTargets)> renderTargets;

	//Container for all pipelines.
	StaticArray<VulkanPipeline *RESTRICT, INDEX(Pipeline::NumberOfPipelines)> pipelines;

	//Container for all semaphores.
	StaticArray<VulkanSemaphore *RESTRICT, INDEX(GraphicsSemaphore::NumberOfSemaphores)> semaphores;

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, INDEX(ShaderModule::NumberOfShaderModules)> shaderModules;

	//Container for all uniform buffers.
	StaticArray<VulkanUniformBuffer *RESTRICT, UniformBuffer::NumberOfUniformBuffers> uniformBuffers;

	//The Vulkan frame data.
	VulkanFrameData frameData;

	//The current command buffer.
	VulkanCommandBuffer *RESTRICT currentCommandBuffer;

	//The current dynamic uniform data descriptor set.
	VulkanDescriptorSet *RESTRICT currentDynamicUniformDataDescriptorSet;

	//The current environment descriptor set.
	VulkanDescriptorSet *RESTRICT currentEnvironmentDataDescriptorSet;

	/*
	*	Initializes all render targets.
	*/
	void InitializeRenderTargets() NOEXCEPT;

	/*
	*	Initializes all semaphores.
	*/
	void InitializeSemaphores() NOEXCEPT;

	/*
	*	Initializes all uniform buffers.
	*/
	void InitializeUniformBuffers() NOEXCEPT;

	/*
	*	Initializes all descriptor set layouts.
	*/
	void InitializeDescriptorSetLayouts() NOEXCEPT;

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() NOEXCEPT;

	/*
	*	Initializes all pipelines.
	*/
	void InitializePipelines() NOEXCEPT;

	/*
	*	Initializes all descriptor sets.
	*/
	void InitializeDescriptorSets() NOEXCEPT;

	/*
	*	Initializes all default textures.
	*/
	void InitializeDefaultTextures() NOEXCEPT;

	/*
	*	Calculates the projection matrix.
	*/
	void CalculateProjectionMatrix() NOEXCEPT;

	/*
	*	Execute asynchronous tasks.
	*/
	void ExecuteAsynchronousTasks() NOEXCEPT;

	/*
	*	Begins the frame.
	*/
	void BeginFrame() NOEXCEPT;

	/*
	*	Executes frame-dependant asynchronous tasks.
	*/
	void ExecuteFrameDependantAsynchronousTasks() NOEXCEPT;

	/*
	*	Concatenates all secondary command buffers into the previous one.
	*/
	void ConcatenateCommandBuffers() NOEXCEPT;

	/*
	*	Ends the frame.
	*/
	void EndFrame() NOEXCEPT;

	/****************************/
	/*	Asynchronous functions. */
	/****************************/

	/*
	*	Updates the properties of particle systems.
	*/
	void UpdateParticleSystemProperties() const NOEXCEPT;

	/*
	*	Updates the vegetation culling.
	*/
	void UpdateVegetationCulling() const NOEXCEPT;

	/*
	*	Updates the view frustum culling.
	*/
	void UpdateViewFrustumCulling() NOEXCEPT;

	/*
	*	Renders directional shadows.
	*/
	void RenderDirectionalShadows() NOEXCEPT;

	/*
	*	Renders the terrain.
	*/
	void RenderTerrain() NOEXCEPT;

	/*
	*	Renders all static physical entities.
	*/
	void RenderStaticPhysicalEntities() NOEXCEPT;

	/*
	*	Renders all instanced physical entities.
	*/
	void RenderInstancedPhysicalEntities() NOEXCEPT;

	/*
	*	Renders all vegetation entities.
	*/
	void RenderVegetationEntities() NOEXCEPT;

	/*
	*	Renders lighting.
	*/
	void RenderLighting() NOEXCEPT;

	/*
	*	Renders skybox.
	*/
	void RenderSkybox() NOEXCEPT;

	/*
	*	Renders all particle system entities.
	*/
	void RenderParticleSystemEntities() NOEXCEPT;

	/*
	*	Renders the ocean.
	*/
	void RenderOcean() NOEXCEPT;

	/*
	*	Renders the post processing.
	*/
	void RenderPostProcessing() NOEXCEPT;

	/*
	*	Updates the descriptor sets.
	*/
	void UpdateDescriptorSets() NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData() NOEXCEPT;

};