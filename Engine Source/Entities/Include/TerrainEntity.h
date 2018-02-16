#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

//Rendering.
#include <RenderingCore.h>

//Forward declarations.
class CPUTexture4;
class TerrainUniformData;

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
	void Initialize(const CPUTexture4 &heightMap, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Texture2DHandle terrainHeightMapTexture, const Texture2DHandle terrainNormalMapTexture, const Texture2DHandle layer1WeightTexture, const Texture2DHandle layer1AlbedoTexture, const Texture2DHandle layer1NormalMapTexture, const Texture2DHandle layer1RoughnessTexture, const Texture2DHandle layer1MetallicTexture, const Texture2DHandle layer1AmbientOcclusionTexture, const Texture2DHandle layer1DisplacementTexture, const Texture2DHandle layer2WeightTexture = nullptr, const Texture2DHandle layer2AlbedoTexture = nullptr, const Texture2DHandle layer2NormalMapTexture = nullptr, const Texture2DHandle layer2RoughnessTexture = nullptr, const Texture2DHandle layer2MetallicTexture = nullptr, const Texture2DHandle layer2AmbientOcclusionTexture = nullptr, const Texture2DHandle layer2DisplacementTexture = nullptr, const Texture2DHandle layer3WeightTexture = nullptr, const Texture2DHandle layer3AlbedoTexture = nullptr, const Texture2DHandle layer3NormalMapTexture = nullptr, const Texture2DHandle layer3RoughnessTexture = nullptr, const Texture2DHandle layer3MetallicTexture = nullptr, const Texture2DHandle layer3AmbientOcclusionTexture = nullptr, const Texture2DHandle layer3DisplacementTexture = nullptr) NOEXCEPT;

};