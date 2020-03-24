#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

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
		Shader _ClosestHitShader;

		//The any hit shader.
		Shader _AnyHitShader;

		//The intersection shader.
		Shader _IntersectionShader;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE constexpr HitGroup(const Shader initial_closest_hit_shader, const Shader initial_any_hit_shader, const Shader initial_intersection_shader) NOEXCEPT
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
	FORCE_INLINE NO_DISCARD Shader GetRayGenerationShader() const NOEXCEPT
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
	*	Sets the number of hit groups.
	*/
	FORCE_INLINE void SetNumberOfHitGroups(const uint64 number_of_hit_groups) NOEXCEPT
	{
		_HitGroups.Reserve(number_of_hit_groups);
	}

	/*
	*	Adds a hit group.
	*/
	FORCE_INLINE void AddHitGroup(const Shader closest_hit_shader, const Shader any_hit_shader, const Shader intersection_shader) NOEXCEPT
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
	FORCE_INLINE void AddMissShader(const Shader shader) NOEXCEPT
	{
		_MissShaders.Emplace(shader);
	}

private:

	//The ray generation shader.
	Shader _RayGenerationShader;

	//The hit groups.
	DynamicArray<HitGroup> _HitGroups;

	//The miss shaders.
	DynamicArray<Shader> _MissShaders;

};