#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

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
	const Texture2DHandle GetAlbedoTexture() const NOEXCEPT { return albedoTexture; }

	/*
	*	Sets the the albedo texture.
	*/
	void SetAlbedoTexture(Texture2DHandle newAlbedoTexture) NOEXCEPT { albedoTexture = newAlbedoTexture; }

	/*
	*	Returns the the normal map texture.
	*/
	const Texture2DHandle GetNormalMapTexture() const NOEXCEPT { return normalMapTexture; }

	/*
	*	Sets the the normal map texture.
	*/
	void SetNormalMapTexture(Texture2DHandle newNormalMapTexture) NOEXCEPT { normalMapTexture = newNormalMapTexture; }

	/*
	*	Returns the the roughness texture.
	*/
	const Texture2DHandle GetRoughnessTexture() const NOEXCEPT { return roughnessTexture; }

	/*
	*	Sets the the roughness texture.
	*/
	void SetRoughnessTexture(Texture2DHandle newRoughnessTexture) NOEXCEPT { roughnessTexture = newRoughnessTexture; }

	/*
	*	Returns the the metallic texture.
	*/
	const Texture2DHandle GetMetallicTexture() const NOEXCEPT { return metallicTexture; }

	/*
	*	Sets the the metallic texture.
	*/
	void SetMetallicTexture(Texture2DHandle newMetallicTexture) NOEXCEPT { metallicTexture = newMetallicTexture; }

	/*
	*	Returns the the ambient occlusion texture.
	*/
	const Texture2DHandle GetAmbientOcclusionTexture() const NOEXCEPT { return ambientOcclusiontexture; }

	/*
	*	Sets the the ambient occlusion texture.
	*/
	void SetAmbientOcclusionTexture(Texture2DHandle newAmbientOcclusionTexture) NOEXCEPT { ambientOcclusiontexture = newAmbientOcclusionTexture; }

private:

	//The albedo texture.
	Texture2DHandle albedoTexture{ nullptr };

	//The normal map texture.
	Texture2DHandle normalMapTexture{ nullptr };

	//The roughness texture.
	Texture2DHandle roughnessTexture{ nullptr };

	//The metallic texture.
	Texture2DHandle metallicTexture{ nullptr };

	//The ambient occlusion texture.
	Texture2DHandle ambientOcclusiontexture{ nullptr };

};