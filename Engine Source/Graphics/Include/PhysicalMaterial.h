#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class Vulkan2DTexture;

class PhysicalMaterial
{

public:

	/*
	*	Default constructor.
	*/
	PhysicalMaterial() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicalMaterial() CATALYST_NOEXCEPT;

	/*
	*	Returns the the albedo texture.
	*/
	CATALYST_RESTRICTED const Vulkan2DTexture* GetAlbedoTexture() const CATALYST_NOEXCEPT { return albedoTexture; }

	/*
	*	Sets the the albedo texture.
	*/
	void SetAlbedoTexture(Vulkan2DTexture *CATALYST_RESTRICT newAlbedoTexture) CATALYST_NOEXCEPT { albedoTexture = newAlbedoTexture; }

	/*
	*	Returns the the normal map texture.
	*/
	CATALYST_RESTRICTED const Vulkan2DTexture* GetNormalMapTexture() const CATALYST_NOEXCEPT { return normalMapTexture; }

	/*
	*	Sets the the normal map texture.
	*/
	void SetNormalMapTexture(Vulkan2DTexture *CATALYST_RESTRICT newNormalMapTexture) CATALYST_NOEXCEPT { normalMapTexture = newNormalMapTexture; }

	/*
	*	Returns the the roughness texture.
	*/
	CATALYST_RESTRICTED const Vulkan2DTexture* GetRoughnessTexture() const CATALYST_NOEXCEPT { return roughnessTexture; }

	/*
	*	Sets the the roughness texture.
	*/
	void SetRoughnessTexture(Vulkan2DTexture *CATALYST_RESTRICT newRoughnessTexture) CATALYST_NOEXCEPT { roughnessTexture = newRoughnessTexture; }

	/*
	*	Returns the the metallic texture.
	*/
	CATALYST_RESTRICTED const Vulkan2DTexture* GetMetallicTexture() const CATALYST_NOEXCEPT { return metallicTexture; }

	/*
	*	Sets the the metallic texture.
	*/
	void SetMetallicTexture(Vulkan2DTexture *CATALYST_RESTRICT newMetallicTexture) CATALYST_NOEXCEPT { metallicTexture = newMetallicTexture; }

	/*
	*	Returns the the ambient occlusion texture.
	*/
	CATALYST_RESTRICTED const Vulkan2DTexture* GetAmbientOcclusionTexture() const CATALYST_NOEXCEPT { return ambientOcclusiontexture; }

	/*
	*	Sets the the ambient occlusion texture.
	*/
	void SetAmbientOcclusionTexture(Vulkan2DTexture *CATALYST_RESTRICT newAmbientOcclusionTexture) CATALYST_NOEXCEPT { ambientOcclusiontexture = newAmbientOcclusionTexture; }

private:

	//The albedo texture.
	Vulkan2DTexture *CATALYST_RESTRICT albedoTexture{ nullptr };

	//The normal map texture.
	Vulkan2DTexture *CATALYST_RESTRICT normalMapTexture{ nullptr };

	//The roughness texture.
	Vulkan2DTexture *CATALYST_RESTRICT roughnessTexture{ nullptr };

	//The metallic texture.
	Vulkan2DTexture *CATALYST_RESTRICT metallicTexture{ nullptr };

	//The ambient occlusion texture.
	Vulkan2DTexture *CATALYST_RESTRICT ambientOcclusiontexture{ nullptr };

};