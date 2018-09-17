//Header file.
#include <Systems/RenderingSystem.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CommonPhysicalModelData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/OceanMaterial.h>
#endif
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/VegetationMaterial.h>
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
#include <Resources/VegetationMaterialData.h>

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

	//Initialize the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeSystem();

	//Initialize the common particle materials.
	InitializeCommonParticleMaterials();

	//Initializes the common physical materials.
	InitializeCommonPhysicalMaterials();

	//Initialize the common physical models.
	InitializeCommonPhysicalModels();

	//Initialize all default assets.
	InitializeDefaultAssets();

	//Initialize all special textures.
	InitializeSpecialTextures();
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
*	Returns the given render target.
*/
RenderTargetHandle RenderingSystem::GetRenderTarget(const RenderTarget renderTarget) const NOEXCEPT
{
	//Return the given render target via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetRenderTarget(renderTarget);
}

/*
*	Returns the given special texture.
*/
Texture2DHandle RenderingSystem::GetSpecialTexture(const SpecialTexture specialTexture) NOEXCEPT
{
	return _SpecialTextures[INDEX(specialTexture)];
}

/*
*	Returns the given uniform buffer.
*/
UniformBufferHandle RenderingSystem::GetUniformBuffer(const UniformBuffer uniformBuffer) NOEXCEPT
{
	return CURRENT_RENDERING_SYSTEM::Instance->GetUniformBuffer(uniformBuffer);
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle RenderingSystem::CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	//Create the constant buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateConstantBuffer(data, dataSizes, dataChunks);
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
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle renderDataTable) const NOEXCEPT
{
	//Destroy the render data table via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyRenderDataTable(renderDataTable);
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
void RenderingSystem::DestroyTexture2D(Texture2DHandle texture) const NOEXCEPT
{
	//Destroy the texture 2D via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->DestroyTexture2D(texture);
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
*	Returns the current dynamic uniform data render data table.
*/
RenderDataTableHandle RenderingSystem::GetCurrentDynamicUniformDataRenderDataTable() const NOEXCEPT
{
	//Return the current dynamic uniform data render data table via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentDynamicUniformDataRenderDataTable();
}

/*
*	Returns the current environment render data table.
*/
RenderDataTableHandle RenderingSystem::GetCurrentEnvironmentRenderDataTable() const NOEXCEPT
{
	//Return the current environment render data table via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentEnvironmentRenderDataTable();
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Returns the current ocean render data table.
*/
RenderDataTableHandle RenderingSystem::GetCurrentOceanRenderDataTable() const NOEXCEPT
{
	//Return the current ocean render data table via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentOceanRenderDataTable();
}
#endif

/*
*	Returns the given common render data table layout.
*/
RenderDataTableHandle RenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT
{
	//Return the given common render data table layout via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCommonRenderDataTableLayout(commonRenderDataTableLayout);
}

/*
*	Creates an environment material.
*/
void RenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the environment material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateEnvironmentMaterial(environmentMaterialData, environmentMaterial);
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Creates an ocean material.
*/
void RenderingSystem::CreateOceanMaterial(const OceanMaterialData &oceanMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT
{
	//Create the normal map texture.
	oceanMaterial._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(oceanMaterialData._NormalMapData, oceanMaterialData._Width, oceanMaterialData._Height, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
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
	CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Physical), &physicalMaterial._RenderDataTable);
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
*	Creates a vegetation material.
*/
void RenderingSystem::CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT
{
	//Create the mask texture.
	vegetationMaterial._MaskTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData._MaskData, vegetationMaterialData._Width, vegetationMaterialData._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the albedo texture.
	vegetationMaterial._AlbedoTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData._AlbedoData, vegetationMaterialData._Width, vegetationMaterialData._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the normal map texture.
	vegetationMaterial._NormalMapTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData._NormalMapData, vegetationMaterialData._Width, vegetationMaterialData._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));

	//Create the properties texture.
	vegetationMaterial._PropertiesTexture = CreateTexture2D(TextureData(TextureDataContainer(vegetationMaterialData._PropertiesData, vegetationMaterialData._Width, vegetationMaterialData._Height, 4), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Linear, TextureFormat::R8G8B8A8_Byte));
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
void RenderingSystem::InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Initialize the terrain entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeTerrainEntity(entity, data);
}

/*
*	Terminates a terrain entity.
*/
void RenderingSystem::TerminateTerrainEntity(const TerrainEntity *const RESTRICT entity) const NOEXCEPT
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
void RenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Initialize the instanced physical entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeInstancedPhysicalEntity(entity, model, transformations);
}

/*
*	Initializes a vegetation entity.
*/
void RenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Initialize the vegetation entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeVegetationEntity(entity, material, transformations, properties);
}

/*
*	Initializes a particle system entity.
*/
void RenderingSystem::InitializeParticleSystemEntity(const Entity *const RESTRICT entity, const ParticleSystemInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Initialize the particle system entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeParticleSystemEntity(entity, data);
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
*	Registers all render passes.
*/
void RenderingSystem::RegisterRenderPasses() NOEXCEPT
{
	//Register all render passes.
	_RenderPasses[INDEX(RenderPassSubStage::DirectionalTerrainShadow)] = DirectionalTerrainShadowRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DirectionalDynamicPhysicalShadow)] = DirectionalDynamicPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DirectionalInstancedPhysicalShadow)] = DirectionalInstancedPhysicalShadowRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::Terrain)] = TerrainRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DynamicPhysical)] = DynamicPhysicalRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::InstancedPhysical)] = InstancedPhysicalRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::Vegetation)] = VegetationRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DirectionalShadow)] = DirectionalShadowRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::Lighting)] = LightingRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::Sky)] = SkyRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DynamicOutline)] = DynamicOutlineRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::ParticleSystem)] = ParticleSystemRenderPass::Instance.Get();
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	_RenderPasses[INDEX(RenderPassSubStage::VolumetricFog)] = VolumetricFogRenderPass::Instance.Get();
#endif
#if !defined(CATALYST_FINAL)
	_RenderPasses[INDEX(RenderPassSubStage::DebugAxisAlignedBoundingBox)] = DebugAxisAlignedBoundingBoxRenderPass::Instance.Get();
	_RenderPasses[INDEX(RenderPassSubStage::DebugScreenBox)] = DebugScreenBoxRenderPass::Instance.Get();
#endif
#if defined(CATALYST_ENABLE_OCEAN)
	_RenderPasses[INDEX(RenderPassSubStage::Ocean)] = OceanRenderPass::Instance.Get();
#endif
	_RenderPasses[INDEX(RenderPassSubStage::PostProcessing)] = PostProcessingRenderPass::Instance.Get();
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
*	Initializes the common particle materials.
*/
void RenderingSystem::InitializeCommonParticleMaterials() NOEXCEPT
{
	{
		//Create the white circle common particle material.
		constexpr uint8 RESOLUTION{ 7 };
		constexpr Vector2 MIDDLE_POINT{ 0.5f, 0.5f };

		ParticleMaterialData data;

		data._MipmapLevels = 1;
		data._Width = RESOLUTION;
		data._Height = RESOLUTION;

		data._AlbedoData.UpsizeSlow(1);
		data._AlbedoData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint8 i = 0; i < RESOLUTION; ++i)
		{
			for (uint8 j = 0; j < RESOLUTION; ++j)
			{
				const Vector2 point{ static_cast<float>(i) / static_cast<float>(RESOLUTION - 1), static_cast<float>(j) / static_cast<float>(RESOLUTION - 1) };
				const float distance{ Vector2::LengthSquared(MIDDLE_POINT - point) * 2.0f };
				float alpha{ 1.0f - distance };
				alpha *= alpha;
				alpha *= alpha;

				data._AlbedoData[0].EmplaceFast(255);
				data._AlbedoData[0].EmplaceFast(255);
				data._AlbedoData[0].EmplaceFast(255);
				data._AlbedoData[0].EmplaceFast(static_cast<byte>(static_cast<float>(255) * alpha));
			}
		}

		CreateParticleMaterial(data, _CommonParticleMaterials[INDEX(CommonParticleMaterial::WhiteCircle)]);
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

		data._MipmapLevels = 1;
		data._Width = 1;
		data._Height = 1;
		data._AlbedoData.UpsizeSlow(1);
		data._AlbedoData[0].Reserve(4);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(255);
		data._NormalMapData.UpsizeSlow(1);
		data._NormalMapData[0].Reserve(4);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(255);
		data._NormalMapData[0].EmplaceFast(255);
		data._MaterialPropertiesData.UpsizeSlow(1);
		data._MaterialPropertiesData[0].Reserve(4);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);

		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[INDEX(CommonPhysicalMaterial::Black)]);
	}

	{
		//Create the red common physical material.
		PhysicalMaterialData data;
		
		data._MipmapLevels = 1;
		data._Width = 1;
		data._Height = 1;
		data._AlbedoData.UpsizeSlow(1);
		data._AlbedoData[0].Reserve(4);
		data._AlbedoData[0].EmplaceFast(255);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(255);
		data._NormalMapData.UpsizeSlow(1);
		data._NormalMapData[0].Reserve(4);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(255);
		data._NormalMapData[0].EmplaceFast(255);
		data._MaterialPropertiesData.UpsizeSlow(1);
		data._MaterialPropertiesData[0].Reserve(4);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);

		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[INDEX(CommonPhysicalMaterial::Red)]);
	}

	{
		//Create the teal common physical material.
		PhysicalMaterialData data;

		data._MipmapLevels = 1;
		data._Width = 1;
		data._Height = 1;
		data._AlbedoData.UpsizeSlow(1);
		data._AlbedoData[0].Reserve(4);
		data._AlbedoData[0].EmplaceFast(0);
		data._AlbedoData[0].EmplaceFast(255);
		data._AlbedoData[0].EmplaceFast(255);
		data._AlbedoData[0].EmplaceFast(255);
		data._NormalMapData.UpsizeSlow(1);
		data._NormalMapData[0].Reserve(4);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(127);
		data._NormalMapData[0].EmplaceFast(255);
		data._NormalMapData[0].EmplaceFast(255);
		data._MaterialPropertiesData.UpsizeSlow(1);
		data._MaterialPropertiesData[0].Reserve(4);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);
		data._MaterialPropertiesData[0].EmplaceFast(255);
		data._MaterialPropertiesData[0].EmplaceFast(0);

		CreatePhysicalMaterial(data, _CommonPhysicalMaterials[INDEX(CommonPhysicalMaterial::Teal)]);
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
		CommonPhysicalModelData::GetCubePhysicalModelData(data);
		CreatePhysicalModel(data, _CommonPhysicalModels[INDEX(CommonPhysicalModel::Cube)]);
	}

	{
		//Create the octahedron common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetOctahedronPhysicalModelData(data);
		CreatePhysicalModel(data, _CommonPhysicalModels[INDEX(CommonPhysicalModel::Octahedron)]);
	}

	{
		//Create the plane common physical model.
		PhysicalModelData data;
		CommonPhysicalModelData::GetPlanePhysicalModelData(data);
		CreatePhysicalModel(data, _CommonPhysicalModels[INDEX(CommonPhysicalModel::Plane)]);
	}
}

/*
*	Initializes all default assets.
*/
void RenderingSystem::InitializeDefaultAssets() NOEXCEPT
{
	{
		//Initialize the default night environment material.
		constexpr Vector3 BOTTOM_COLOR{ 0.025f, 0.025f, 0.025f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f / 64.0f, 1.0f / 64.0f };

		EnvironmentMaterialData data;

		data._DiffuseResolution = 2;
		data._DiffuseData.Reserve(16 * 6);
		data._DiffuseIrradianceResolution = 2;
		data._DiffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}
		}

		CreateEnvironmentMaterial(data, _DefaultNightEnvironmentMaterial);

		EnvironmentManager::Instance->SetNightEnvironmentMaterial(_DefaultNightEnvironmentMaterial);
	}

	{
		//Initialize the default day environment material.
		//constexpr Vector3 BOTTOM_COLOR{ 0.25f, 0.25f, 0.25f };
		//constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f, 1.0f };

		constexpr Vector3 BOTTOM_COLOR{ 0.0f, 0.0f, 0.0f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.0f, 0.0f };

		EnvironmentMaterialData data;

		data._DiffuseResolution = 2;
		data._DiffuseData.Reserve(16 * 6);
		data._DiffuseIrradianceResolution = 2;
		data._DiffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);

				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data._DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data._DiffuseIrradianceData.EmplaceFast(1.0f);
			}
		}

		CreateEnvironmentMaterial(data, _DefaultDayEnvironmentMaterial);

		EnvironmentManager::Instance->SetDayEnvironmentMaterial(_DefaultDayEnvironmentMaterial);
	}

	//Set the environment blend to be day by default.
	EnvironmentManager::Instance->SetEnvironmentBlend(1.0f);

#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Initialize the default ocean material.
		constexpr uint8 RESOLUTION{ 32 };

		OceanMaterialData data;

		data._MipmapLevels = 1;
		data._Width = RESOLUTION;
		data._Height = RESOLUTION;
		data._NormalMapData.UpsizeSlow(1);
		data._NormalMapData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint16 i = 0; i < RESOLUTION * RESOLUTION; ++i)
		{
			data._NormalMapData[0].EmplaceFast(static_cast<uint8>(CatalystBaseMath::RandomIntegerInRange<uint16>(126, 128)));
			data._NormalMapData[0].EmplaceFast(static_cast<uint8>(CatalystBaseMath::RandomIntegerInRange<uint16>(126, 128)));
			data._NormalMapData[0].EmplaceFast(255);
			data._NormalMapData[0].EmplaceFast(255);
		}

		CreateOceanMaterial(data, _DefaultOceanMaterial);

		EnvironmentManager::Instance->SetOceanMaterial(_DefaultOceanMaterial);
	}
#endif
}

/*
*	Initializes all special textures.
*/
void RenderingSystem::InitializeSpecialTextures() NOEXCEPT
{
	{
		//Initialize the screen space ambient occlusion random kernel texture.
		StaticArray<Vector4, RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_RANDOM_KERNEL_SIZE> samples;

		for (Vector4& sample : samples)
		{
			sample._X = CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Y = CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Z = 0.0f;
			sample._W = 0.0f;
		}

		_SpecialTextures[INDEX(SpecialTexture::ScreenSpaceAmbientOcclusionRandomNoise)] = CreateTexture2D(TextureData(TextureDataContainer(samples, 4, 4), AddressMode::Repeat, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));
	}
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM