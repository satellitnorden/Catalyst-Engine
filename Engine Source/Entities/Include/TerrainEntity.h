#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

//Forward declarations.
class CPUTexture4;
class TerrainUniformData;
class Vulkan2DTexture;

class TerrainEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(TerrainEntity);

	/*
	*	Default constructor.
	*/
	TerrainEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~TerrainEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) NOEXCEPT final override;

	/*
	*	Initializes this terrain entity.
	*/
	void Initialize(const CPUTexture4 &heightMap, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Vulkan2DTexture *RESTRICT terrainHeightMapTexture, const Vulkan2DTexture *RESTRICT terrainNormalMapTexture, const Vulkan2DTexture *RESTRICT layer1WeightTexture, const Vulkan2DTexture *RESTRICT layer1AlbedoTexture, const Vulkan2DTexture *RESTRICT layer1NormalMapTexture, const Vulkan2DTexture *RESTRICT layer1RoughnessTexture, const Vulkan2DTexture *RESTRICT layer1MetallicTexture, const Vulkan2DTexture *RESTRICT layer1AmbientOcclusionTexture, const Vulkan2DTexture *RESTRICT layer1DisplacementTexture, const Vulkan2DTexture *RESTRICT layer2WeightTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2AlbedoTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2NormalMapTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2RoughnessTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2MetallicTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2AmbientOcclusionTexture = nullptr, const Vulkan2DTexture *RESTRICT layer2DisplacementTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3WeightTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3AlbedoTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3NormalMapTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3RoughnessTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3MetallicTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3AmbientOcclusionTexture = nullptr, const Vulkan2DTexture *RESTRICT layer3DisplacementTexture = nullptr) NOEXCEPT;

};