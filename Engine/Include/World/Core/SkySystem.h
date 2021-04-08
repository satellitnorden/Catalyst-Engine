#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Optional.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/TextureCubeResource.h>

//World.
#include <World/Sky/SkyGradient.h>

class SkySystem final
{

public:

	//Enumeration covering all sky modes.
	enum class SkyMode : uint8
	{
		GRADIENT,
		TEXTURE
	};

	/*
	*	Post-initializes the sky system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns the sky mode.
	*/
	FORCE_INLINE NO_DISCARD SkyMode GetSkyMode() const NOEXCEPT
	{
		return _SkyMode;
	}

	/*
	*	Returns the sky gradient.
	*/
	FORCE_INLINE NO_DISCARD const SkyGradient &GetSkyGradient() const NOEXCEPT
	{
		return _SkyGradient;
	}

	/*
	*	Sets the sky gradient.
	*/
	FORCE_INLINE void SetSkyGradient(const SkyGradient &value) NOEXCEPT
	{
		_SkyMode = SkyMode::GRADIENT;
		_SkyGradient = value;
	}

	/*
	*	Returns the sky texture.
	*/
	FORCE_INLINE NO_DISCARD const ResourcePointer<TextureCubeResource> GetSkyTexture() const NOEXCEPT
	{
		return _SkyTexture;
	}

	/*
	*	Sets the sky texture.
	*/
	FORCE_INLINE void SetSkyTexture(const ResourcePointer<TextureCubeResource> value) NOEXCEPT
	{
		_SkyMode = SkyMode::TEXTURE;
		_SkyTexture = value;
	}

	/*
	*	Returns the sky intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSkyIntensity() const NOEXCEPT
	{
		return _SkyIntensity;
	}

	/*
	*	Sets the sky intensity.
	*/
	FORCE_INLINE void SetSkyIntensity(const float32 value) NOEXCEPT
	{
		_SkyIntensity = value;
	}

	/*
	*	Returns the cloud thickness.
	*/
	FORCE_INLINE NO_DISCARD float32 GetCloudThickness() const NOEXCEPT
	{
		return _CloudThickness;
	}

	/*
	*	Sets the cloud thickness.
	*/
	FORCE_INLINE void SetCloudThickness(const float32 value) NOEXCEPT
	{
		_CloudThickness = value;
	}

private:

	//The sky mode.
	SkyMode _SkyMode{ SkyMode::TEXTURE };

	//The sky gradient.
	SkyGradient _SkyGradient;

	//The sky texture.
	ResourcePointer<TextureCubeResource> _SkyTexture;

	//The sky intensity.
	float32 _SkyIntensity{ 1.0f };

	//The cloud thickness.
	float32 _CloudThickness{ 0.0f };

};