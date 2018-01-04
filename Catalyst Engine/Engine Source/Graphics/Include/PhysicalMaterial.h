#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class VulkanTexture;

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
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanTexture* GetAlbedoTexture() const CATALYST_NOEXCEPT { return albedoTexture; }

	/*
	*	Sets the the albedo texture.
	*/
	CATALYST_FORCE_INLINE void SetAlbedoTexture(VulkanTexture *CATALYST_RESTRICT newAlbedoTexture) CATALYST_NOEXCEPT { albedoTexture = newAlbedoTexture; }

	/*
	*	Returns the the normal map texture.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanTexture* GetNormalMapTexture() const CATALYST_NOEXCEPT { return normalMapTexture; }

	/*
	*	Sets the the normal map texture.
	*/
	CATALYST_FORCE_INLINE void SetNormalMapTexture(VulkanTexture *CATALYST_RESTRICT newNormalMapTexture) CATALYST_NOEXCEPT { normalMapTexture = newNormalMapTexture; }

	/*
	*	Returns the the roughness texture.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanTexture* GetRoughnessTexture() const CATALYST_NOEXCEPT { return roughnessTexture; }

	/*
	*	Sets the the roughness texture.
	*/
	CATALYST_FORCE_INLINE void SetRoughnessTexture(VulkanTexture *CATALYST_RESTRICT newRoughnessTexture) CATALYST_NOEXCEPT { roughnessTexture = newRoughnessTexture; }

	/*
	*	Returns the the metallic texture.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanTexture* GetMetallicTexture() const CATALYST_NOEXCEPT { return metallicTexture; }

	/*
	*	Sets the the metallic texture.
	*/
	CATALYST_FORCE_INLINE void SetMetallicTexture(VulkanTexture *CATALYST_RESTRICT newMetallicTexture) CATALYST_NOEXCEPT { metallicTexture = newMetallicTexture; }

	/*
	*	Returns the the ambient occlusion texture.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanTexture* GetAmbientOcclusionTexture() const CATALYST_NOEXCEPT { return ambientOcclusiontexture; }

	/*
	*	Sets the the ambient occlusion texture.
	*/
	CATALYST_FORCE_INLINE void SetAmbientOcclusionTexture(VulkanTexture *CATALYST_RESTRICT newAmbientOcclusionTexture) CATALYST_NOEXCEPT { ambientOcclusiontexture = newAmbientOcclusionTexture; }

private:

	//The albedo texture.
	VulkanTexture *CATALYST_RESTRICT albedoTexture{ nullptr };

	//The normal map texture.
	VulkanTexture *CATALYST_RESTRICT normalMapTexture{ nullptr };

	//The roughness texture.
	VulkanTexture *CATALYST_RESTRICT roughnessTexture{ nullptr };

	//The metallic texture.
	VulkanTexture *CATALYST_RESTRICT metallicTexture{ nullptr };

	//The ambient occlusion texture.
	VulkanTexture *CATALYST_RESTRICT ambientOcclusiontexture{ nullptr };

};