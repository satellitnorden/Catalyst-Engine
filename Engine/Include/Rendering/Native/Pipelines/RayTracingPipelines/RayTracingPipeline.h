#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

class RayTracingPipeline : public Pipeline
{

public:

	/*
	*	Default constructor.
	*/
	RayTracingPipeline() NOEXCEPT;

	/*
	*	Returns the ray generation shader.
	*/
	FORCE_INLINE NO_DISCARD Shader GetRayGenerationShader() const NOEXCEPT
	{
		return _RayGenerationShader;
	}

	/*
	*	Returns the closest shader.
	*/
	FORCE_INLINE NO_DISCARD Shader GetClosestHitShader() const NOEXCEPT
	{
		return _ClosestHitShader;
	}

	/*
	*	Returns the miss shader.
	*/
	FORCE_INLINE NO_DISCARD Shader GetMissShader() const NOEXCEPT
	{
		return _MissShader;
	}

protected:

	/*
	*	Sets the ray generation shader.
	*/
	FORCE_INLINE void SetRayGenerationShader(const Shader shader) NOEXCEPT
	{
		_RayGenerationShader = shader;
	}

	/*
	*	Sets the closest shader.
	*/
	FORCE_INLINE void SetClosestHitShader(const Shader shader) NOEXCEPT
	{
		_ClosestHitShader = shader;
	}

	/*
	*	Sets the miss shader.
	*/
	FORCE_INLINE void SetMissShader(const Shader shader) NOEXCEPT
	{
		_MissShader = shader;
	}

private:

	//The ray generation shader.
	Shader _RayGenerationShader;

	//The closest hit shader.
	Shader _ClosestHitShader;

	//The miss shader.
	Shader _MissShader;

};