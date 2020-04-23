#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/DebugRenderingSystem.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Initializes the debug rendering system.
*/
void DebugRenderingSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<DebugRenderingSystem *const RESTRICT>(arguments)->PostUpdate();
	},
	this,
	UpdatePhase::POST,
	UpdatePhase::PRE,
	false);
}

/*
*	Debug renders a sphere.
*/
void DebugRenderingSystem::DebugRenderSphere(	const Sphere &sphere,
												const Vector4<float32> &color,
												const uint32 number_of_segments,
												const float32 lifetime) NOEXCEPT
{
	//Add the debug sphere render.
	DebugSphereRender debug_sphere_render;

	debug_sphere_render._Sphere = sphere;
	debug_sphere_render._Color = color;
	debug_sphere_render._NumberOfSegments = number_of_segments;
	debug_sphere_render._CurrentLifetime = 0.0f;
	debug_sphere_render._MaximumLifetime = lifetime;

	{
		SCOPED_LOCK(_DebugSphereRendersLock);

		_DebugSphereRenders.Emplace(debug_sphere_render);
	}
}

/*
*	Updates the debug rendering system during the post update phase.
*/
void DebugRenderingSystem::PostUpdate() NOEXCEPT
{
	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update all debug sphere renders.
	{
		SCOPED_LOCK(_DebugSphereRendersLock);

		for (uint64 i{ 0 }; i < _DebugSphereRenders.Size();)
		{
			_DebugSphereRenders[i]._CurrentLifetime += delta_time;

			if (_DebugSphereRenders[i]._CurrentLifetime >= _DebugSphereRenders[i]._MaximumLifetime)
			{
				_DebugSphereRenders.EraseAt(i);
			}

			else
			{
				++i;
			}
		}
	}
}
#endif