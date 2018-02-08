#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class Vulkan2DTexture;

class PhysicalMaterial final
{

public:

	/*
	*	Default constructor.
	*/
	PhysicalMaterial() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicalMaterial() NOEXCEPT;

	/*
	*	Returns the the albedo texture.
	*/
	RESTRICTED const Vulkan2DTexture* GetAlbedoTexture() const NOEXCEPT { return albedoTexture; }

	/*
	*	Sets the the albedo texture.
	*/
	void SetAlbedoTexture(Vulkan2DTexture *RESTRICT newAlbedoTexture) NOEXCEPT { albedoTexture = newAlbedoTexture; }

	/*
	*	Returns the the normal map texture.
	*/
	RESTRICTED const Vulkan2DTexture* GetNormalMapTexture() const NOEXCEPT { return normalMapTexture; }

	/*
	*	Sets the the normal map texture.
	*/
	void SetNormalMapTexture(Vulkan2DTexture *RESTRICT newNormalMapTexture) NOEXCEPT { normalMapTexture = newNormalMapTexture; }

	/*
	*	Returns the the roughness texture.
	*/
	RESTRICTED const Vulkan2DTexture* GetRoughnessTexture() const NOEXCEPT { return roughnessTexture; }

	/*
	*	Sets the the roughness texture.
	*/
	void SetRoughnessTexture(Vulkan2DTexture *RESTRICT newRoughnessTexture) NOEXCEPT { roughnessTexture = newRoughnessTexture; }

	/*
	*	Returns the the metallic texture.
	*/
	RESTRICTED const Vulkan2DTexture* GetMetallicTexture() const NOEXCEPT { return metallicTexture; }

	/*
	*	Sets the the metallic texture.
	*/
	void SetMetallicTexture(Vulkan2DTexture *RESTRICT newMetallicTexture) NOEXCEPT { metallicTexture = newMetallicTexture; }

	/*
	*	Returns the the ambient occlusion texture.
	*/
	RESTRICTED const Vulkan2DTexture* GetAmbientOcclusionTexture() const NOEXCEPT { return ambientOcclusiontexture; }

	/*
	*	Sets the the ambient occlusion texture.
	*/
	void SetAmbientOcclusionTexture(Vulkan2DTexture *RESTRICT newAmbientOcclusionTexture) NOEXCEPT { ambientOcclusiontexture = newAmbientOcclusionTexture; }

private:

	//The albedo texture.
	Vulkan2DTexture *RESTRICT albedoTexture{ nullptr };

	//The normal map texture.
	Vulkan2DTexture *RESTRICT normalMapTexture{ nullptr };

	//The roughness texture.
	Vulkan2DTexture *RESTRICT roughnessTexture{ nullptr };

	//The metallic texture.
	Vulkan2DTexture *RESTRICT metallicTexture{ nullptr };

	//The ambient occlusion texture.
	Vulkan2DTexture *RESTRICT ambientOcclusiontexture{ nullptr };

};