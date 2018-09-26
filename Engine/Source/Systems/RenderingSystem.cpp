//Header file.
#include <Systems/RenderingSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>
#include <Entities/InitializationData/TerrainInitializationData.h>

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
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Resources/OceanMaterialData.h>
#endif
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/TerrainMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/InputSystem.h>

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

/*
*	Updates the rendering system synchronously during the rendering update phase.
*/
void RenderingSystem::RenderingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
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
*	Sets the active camera.
*/
void RenderingSystem::SetActiveCamera(CameraEntity *const RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera.
	_ActiveCamera = newActiveCamera;

	//Update the matrices.
	UpdateMatrices();
}

/*
*	Updates the matrices.
*/
void RenderingSystem::UpdateMatrices() NOEXCEPT
{
	if (_ActiveCamera)
	{
		//Calculate the projection matrix.
		_ProjectionMatrix = Matrix4::Perspective(CatalystBaseMath::DegreesToRadians(_ActiveCamera->GetFieldOfView()), static_cast<float>(GetResolution()._Width) / static_cast<float>(GetResolution()._Height), _ActiveCamera->GetNearPlane(), _ActiveCamera->GetFarPlane());

		//Calculate the camera matrix.
		_CameraMatrix = Matrix4::LookAt(_ActiveCamera->GetPosition(), _ActiveCamera->GetPosition() + _ActiveCamera->GetForwardVector(), _ActiveCamera->GetUpVector());

		//Calculate the view matrix.
		_ViewMatrix = _ProjectionMatrix * _CameraMatrix;

		//Calculate the inverse projection matrix.
		_InverseProjectionMatrix = _ProjectionMatrix;
		_InverseProjectionMatrix.Inverse();

		//Calculate the inverse camera matrix.
		_InverseCameraMatrix = _CameraMatrix;
		_InverseCameraMatrix.Inverse();
	}
}

/*
*	Given screen coordinates, returns the world direction from the camera to where the screen coordinates are pointing.
*/
Vector3 RenderingSystem::GetWorldDirectionFromScreenCoordinate(const Vector2 &coordinates) const NOEXCEPT
{
	if (_ActiveCamera)
	{
		const Vector2 nearPlaneCoordinate{ coordinates._X * 2.0f - 1.0f, (1.0f - coordinates._Y) * 2.0f - 1.0f };
		const Vector3 screenSpacePosition{ nearPlaneCoordinate, 1.0f };
		Vector4 viewSpacePosition{ _InverseProjectionMatrix * Vector4(screenSpacePosition, 1.0f) };
		viewSpacePosition._X /= viewSpacePosition._W;
		viewSpacePosition._Y /= viewSpacePosition._W;
		viewSpacePosition._Z /= viewSpacePosition._W;
		Vector4 worldSpacePosition{ _InverseCameraMatrix * viewSpacePosition };

		return Vector3::Normalize(Vector3(worldSpacePosition._X, worldSpacePosition._Y, worldSpacePosition._Z) - _ActiveCamera->GetPosition());
	}
	
	else
	{
		return Vector3{ 0.0f, 0.0f, -1.0f };
	}
}

/*
*	Returns whether an axis-aligned bounding box is clicked or touched.
*/
bool RenderingSystem::IsClockedOrTouched(const AxisAlignedBoundingBox &box) const NOEXCEPT
{
	//Retrieve the state.
#if defined(CATALYST_WINDOWS)
	const MouseState *const RESTRICT state{ InputSystem::Instance->GetMouseState() };
	const ButtonState buttonState{ state->_Left };
#elif defined(CATALYST_ANDROID)
	const TouchState *const RESTRICT state{ InputSystem::Instance->GetTouchState() };
	const ButtonState buttonState{ state->_ButtonState };
#else
	#error "Function not implemented for platform."
#endif

	//If nothing is clicked or touched, just return false.
	if (buttonState == ButtonState::Pressed || buttonState == ButtonState::PressedHold)
	{
		//Convert the world positions to screen coordinates.
		Vector2 minimumScreenCoordinate;
		Vector2 maximumScreenCoordinate;

		ToScreenCoordinate(box._Minimum, &minimumScreenCoordinate);
		ToScreenCoordinate(box._Maximum, &maximumScreenCoordinate);

		return	state->_CurrentX >= minimumScreenCoordinate._X
				&& state->_CurrentX <= maximumScreenCoordinate._X
				&& state->_CurrentY >= minimumScreenCoordinate._Y
				&& state->_CurrentY <= maximumScreenCoordinate._Y;
	}

	else
	{
		return false;
	}
}

/*
*	Converts a world position to screen coordinates.
*/
void RenderingSystem::ToScreenCoordinate(const Vector3 &worldPosition, Vector2 *const RESTRICT screenCoordinates) const NOEXCEPT
{
	Vector4 viewSpacePosition{ _ViewMatrix * Vector4(worldPosition, 1.0f) };

	const float inverseW{ 1.0f / viewSpacePosition._W };

	viewSpacePosition._X *= inverseW;
	viewSpacePosition._Y *= inverseW;

	screenCoordinates->_X = (viewSpacePosition._X + 1.0f) * 0.5f;
	screenCoordinates->_Y = 1.0f - ((viewSpacePosition._Y + 1.0f) * 0.5f);
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
*	Updates a render data table.
*/
void RenderingSystem::UpdateRenderDataTable(const RenderDataTableUpdateInformation information, RenderDataTableHandle handle) const NOEXCEPT
{
	//Update the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->UpdateRenderDataTable(information, handle);
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
	UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::TextureCube, material._DiffuseTexture), material._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(1, RenderDataTableUpdateInformation::Type::TextureCube, material._DiffuseIrradianceTexture), material._RenderDataTable);
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Creates an ocean material.
*/
void RenderingSystem::CreateOceanMaterial(const OceanMaterialData &data, OceanMaterial &material) const NOEXCEPT
{
	//Create the normal map texture.
	material._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(data._NormalMapData, data._Width, data._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::OceanMaterial), &material._RenderDataTable);

	//Update the render data table.
	UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::Texture2D, material._NormalMapTexture), material._RenderDataTable);
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
	UpdateRenderDataTable(RenderDataTableUpdateInformation(1, RenderDataTableUpdateInformation::Type::Texture2D, physicalMaterial._AlbedoTexture), physicalMaterial._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(2, RenderDataTableUpdateInformation::Type::Texture2D, physicalMaterial._NormalMapTexture), physicalMaterial._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(3, RenderDataTableUpdateInformation::Type::Texture2D, physicalMaterial._MaterialPropertiesTexture), physicalMaterial._RenderDataTable);
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
*	Initializes a terrain entity.
*/
void RenderingSystem::InitializeTerrainEntity(const Entity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Fill the terrain entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetTerrainFrustumCullingComponents()[entity->_ComponentsIndex] };
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainTerrainComponents()[entity->_ComponentsIndex] };
	TerrainRenderComponent &renderComponent{ ComponentManager::GetTerrainTerrainRenderComponents()[entity->_ComponentsIndex] };

	frustumCullingComponent._ModelSpaceAxisAlignedBoundingBox = data->_AxisAlignedBoundingBox;

	Texture2DHandle terrainPropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(data->_TerrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));

	terrainComponent._TerrainUniformData = data->_TerrainUniformData;
	terrainComponent._UniformBuffer = CreateUniformBuffer(sizeof(TerrainUniformData));
	UploadDataToUniformBuffer(terrainComponent._UniformBuffer, &terrainComponent._TerrainUniformData);
	terrainComponent._TerrainProperties = data->_TerrainProperties;
	terrainComponent._TerrainPropertiesTexture = terrainPropertiesTexture;

	//Create the render data table.
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Terrain), &renderComponent._RenderDataTable);

	UpdateRenderDataTable(RenderDataTableUpdateInformation(1, RenderDataTableUpdateInformation::Type::UniformBuffer, terrainComponent._UniformBuffer), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(2, RenderDataTableUpdateInformation::Type::Texture2D, terrainPropertiesTexture), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(3, RenderDataTableUpdateInformation::Type::Texture2D, data->_LayerWeightsTexture), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(4, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FirstLayerAlbedo), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(5, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FirstLayerNormalMap), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(6, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FirstLayerMaterialProperties), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(7, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._SecondLayerAlbedo), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(8, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._SecondLayerNormalMap), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(9, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._SecondLayerMaterialProperties), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(10, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._ThirdLayerAlbedo), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(11, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._ThirdLayerNormalMap), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(12, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._ThirdLayerMaterialProperties), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(13, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FourthLayerAlbedo), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(14, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FourthLayerNormalMap), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(15, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FourthLayerMaterialProperties), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(16, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FifthLayerAlbedo), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(17, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FifthLayerNormalMap), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(18, RenderDataTableUpdateInformation::Type::Texture2D, data->_TerrainMaterial._FifthLayerMaterialProperties), renderComponent._RenderDataTable);

	DynamicArray<float> vertices;
	DynamicArray<uint32> indices;

	RenderingUtilities::GenerateTerrainPlane(128, vertices, indices);

	StaticArray<void *RESTRICT, 2> bufferData;

	bufferData[0] = vertices.Data();
	bufferData[1] = indices.Data();

	StaticArray<uint64, 2> bufferDataSizes;

	bufferDataSizes[0] = sizeof(float) * vertices.Size();
	bufferDataSizes[1] = sizeof(uint32) * indices.Size();

	renderComponent._Buffer = CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);

	renderComponent._IndexOffset = sizeof(float) * vertices.Size();
	renderComponent._IndexCount = static_cast<uint32>(indices.Size());
	renderComponent._IsInViewFrustum = true;
}

/*
*	Terminates a terrain entity.
*/
void RenderingSystem::TerminateTerrainEntity(const Entity *const RESTRICT entity) const NOEXCEPT
{
	//Retrieve the component.
	TerrainComponent &component{ ComponentManager::GetTerrainTerrainComponents()[entity->_ComponentsIndex] };
	TerrainRenderComponent &renderComponent{ ComponentManager::GetTerrainTerrainRenderComponents()[entity->_ComponentsIndex] };

	//Destroy the terrain entity's resources.
	DestroyTexture2D(component._TerrainPropertiesTexture);
	DestroyRenderDataTable(renderComponent._RenderDataTable);
}

/*
*	Initializes an instanced physical entity.
*/
void RenderingSystem::InitializeInstancedPhysicalEntity(const Entity *const RESTRICT entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Cache relevant data.
	InstancedPhysicalRenderComponent &renderComponent{ ComponentManager::GetInstancedPhysicalInstancedPhysicalRenderComponents()[entity->_ComponentsIndex] };

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ transformations.Data() };
	const uint64 transformationsDataSizes[]{ sizeof(Matrix4) * transformations.Size() };

	//Fill the instanced physical entity components with the relevant data.
	renderComponent._ModelBuffer = model._Buffer;
	renderComponent._RenderDataTable = GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red)._RenderDataTable;
	renderComponent._TransformationsBuffer = CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);
	renderComponent._IndexOffset = model._IndexOffset;
	renderComponent._IndexCount = model._IndexCount;
	renderComponent._InstanceCount = static_cast<uint32>(transformations.Size());
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
	component._Properties = data->_ParticleSystemProperties;
	component._PropertiesUniformBuffer = CreateUniformBuffer(sizeof(ParticleSystemProperties));
	UploadDataToUniformBuffer(component._PropertiesUniformBuffer, &component._Properties);
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem), &renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::UniformBuffer, component._PropertiesUniformBuffer), renderComponent._RenderDataTable);
	UpdateRenderDataTable(RenderDataTableUpdateInformation(1, RenderDataTableUpdateInformation::Type::Texture2D, data->_Material._AlbedoTexture), renderComponent._RenderDataTable);
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
	CreateRenderTarget(GetDirectionalShadowMapResolution(), TextureFormat::R8_Byte, TextureFilter::Linear, AddressMode::ClampToBorder, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadowMap)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, TextureFilter::Nearest, AddressMode::ClampToBorder, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadow)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, TextureFilter::Linear, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferAlbedo)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferNormalDepth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferMaterialProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Nearest, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::SceneIntermediate)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, TextureFilter::Linear, AddressMode::ClampToEdge, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
}

/*
*	Registers all render passes.
*/
void RenderingSystem::RegisterRenderPasses() NOEXCEPT
{
	//Register all render passes.
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalDynamicPhysicalShadow)] = DirectionalDynamicPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalInstancedPhysicalShadow)] = DirectionalInstancedPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Terrain)] = TerrainRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicPhysical)] = DynamicPhysicalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::InstancedPhysical)] = InstancedPhysicalRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Lighting)] = LightingRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::Sky)] = SkyRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DynamicOutline)] = DynamicOutlineRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	_RenderPasses[UNDERLYING(RenderPassSubStage::VolumetricFog)] = VolumetricFogRenderPass::Instance.Get();
#endif
#if !defined(CATALYST_FINAL)
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugAxisAlignedBoundingBox)] = DebugAxisAlignedBoundingBoxRenderPass::Instance.Get();
	_RenderPasses[UNDERLYING(RenderPassSubStage::DebugScreenBox)] = DebugScreenBoxRenderPass::Instance.Get();
#endif
#if defined(CATALYST_ENABLE_OCEAN)
	_RenderPasses[UNDERLYING(RenderPassSubStage::Ocean)] = OceanRenderPass::Instance.Get();
#endif
	_RenderPasses[UNDERLYING(RenderPassSubStage::PostProcessing)] = PostProcessingRenderPass::Instance.Get();
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
		//Initialize the terrain render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 18> bindings
		{
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::UniformBuffer, ShaderStage::Vertex | ShaderStage::TessellationControl | ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Vertex | ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(9, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(10, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(11, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(12, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(13, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(14, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(15, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(16, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(17, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(18, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::TessellationEvaluation | ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Terrain)]);
	}

	{
		//Initialize the physical render data table layout.
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
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
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
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM