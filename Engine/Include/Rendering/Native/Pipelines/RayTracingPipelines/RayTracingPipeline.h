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
	*	Returns the miss shaders.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Shader>& GetMissShaders() const NOEXCEPT
	{
		return _MissShaders;
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
	*	Sets the number of miss shaders.
	*/
	FORCE_INLINE void SetNumberOfMissShaders(const uint64 numberOfMissShaders) NOEXCEPT
	{
		_MissShaders.Reserve(numberOfMissShaders);
	}

	/*
	*	Adds a miss shader.
	*/
	FORCE_INLINE void AddMissShader(const Shader shader) NOEXCEPT
	{
		_MissShaders.Emplace(shader);
	}

private:

	//The ray generation shader.
	Shader _RayGenerationShader;

	//The closest hit shader.
	Shader _ClosestHitShader;

	//The miss shaders.
	DynamicArray<Shader> _MissShaders;

};