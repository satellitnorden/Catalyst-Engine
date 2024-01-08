#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>

class RayTracingPipeline : public Pipeline
{

public:

	/*
	*	Hit group class definition.
	*/
	class HitGroup final
	{

	public:

		//The closest hit shader.
		ShaderHandle _ClosestHitShader;

		//The any hit shader.
		ShaderHandle _AnyHitShader;

		//The intersection shader.
		ShaderHandle _IntersectionShader;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE HitGroup(const ShaderHandle initial_closest_hit_shader, const ShaderHandle initial_any_hit_shader, const ShaderHandle initial_intersection_shader) NOEXCEPT
			:
			_ClosestHitShader(initial_closest_hit_shader),
			_AnyHitShader(initial_any_hit_shader),
			_IntersectionShader(initial_intersection_shader)
		{

		}

	};

	/*
	*	Default constructor.
	*/
	RayTracingPipeline() NOEXCEPT;

	/*
	*	Returns the ray generation shader.
	*/
	FORCE_INLINE NO_DISCARD ShaderHandle GetRayGenerationShader() const NOEXCEPT
	{
		return _RayGenerationShader;
	}

	/*
	*	Returns the hit groups.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<HitGroup>& GetHitGroups() const NOEXCEPT
	{
		return _HitGroups;
	}

	/*
	*	Returns the miss shaders.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<ShaderHandle> &GetMissShaders() const NOEXCEPT
	{
		return _MissShaders;
	}

protected:

	/*
	*	Resets this ray tracing pipeline.
	*/
	void ResetRayTracingPipeline() NOEXCEPT;

	/*
	*	Sets the ray generation shader.
	*/
	FORCE_INLINE void SetRayGenerationShader(const ShaderHandle shader) NOEXCEPT
	{
		_RayGenerationShader = shader;
	}

	/*
	*	Sets the number of hit groups.
	*/
	FORCE_INLINE void SetNumberOfHitGroups(const uint64 number_of_hit_groups) NOEXCEPT
	{
		_HitGroups.Reserve(number_of_hit_groups);
	}

	/*
	*	Adds a hit group.
	*/
	FORCE_INLINE void AddHitGroup(const ShaderHandle closest_hit_shader, const ShaderHandle any_hit_shader, const ShaderHandle intersection_shader) NOEXCEPT
	{
		_HitGroups.Emplace(closest_hit_shader, any_hit_shader, intersection_shader);
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
	FORCE_INLINE void AddMissShader(const ShaderHandle shader) NOEXCEPT
	{
		_MissShaders.Emplace(shader);
	}

private:

	//The ray generation shader.
	ShaderHandle _RayGenerationShader;

	//The hit groups.
	DynamicArray<HitGroup> _HitGroups;

	//The miss shaders.
	DynamicArray<ShaderHandle> _MissShaders;

};