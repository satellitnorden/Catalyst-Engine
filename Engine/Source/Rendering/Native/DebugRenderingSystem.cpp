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
*	Debug renders an axis aligned bounding box 3D.
*/
void DebugRenderingSystem::DebugRenderAxisAlignedBoundingBox3D(	const Vector4<float32> &color,
																const bool depth_test,
																const bool wireframe,
																const AxisAlignedBoundingBox3D &axis_aligned_bounding_box_3D,
																const float32 lifetime) NOEXCEPT
{
	//Add the debug axis aligned bounding box 3D render.
	DebugAxisAlignedBoundingBox3DRender debug_axis_aligned_bounding_box_3D_render;

	debug_axis_aligned_bounding_box_3D_render._DebugRender._Color = color;
	debug_axis_aligned_bounding_box_3D_render._DebugRender._DepthTest = depth_test;
	debug_axis_aligned_bounding_box_3D_render._DebugRender._Wireframe = wireframe;
	debug_axis_aligned_bounding_box_3D_render._AxisAlignedBoundingBox3D = axis_aligned_bounding_box_3D;
	debug_axis_aligned_bounding_box_3D_render._CurrentLifetime = 0.0f;
	debug_axis_aligned_bounding_box_3D_render._MaximumLifetime = lifetime;

	{
		SCOPED_LOCK(_DebugAxisAlignedBoundingBox3DRendersLock);

		_DebugAxisAlignedBoundingBox3DRenders.Emplace(debug_axis_aligned_bounding_box_3D_render);
	}
}

/*
*	Debug renders a sphere.
*/
void DebugRenderingSystem::DebugRenderSphere(	const Vector4<float32> &color,
												const bool depth_test,
												const bool wireframe,
												const Sphere &sphere,
												const uint32 number_of_segments,
												const float32 lifetime) NOEXCEPT
{
	//Add the debug sphere render.
	DebugSphereRender debug_sphere_render;

	debug_sphere_render._DebugRender._Color = color;
	debug_sphere_render._DebugRender._DepthTest = depth_test;
	debug_sphere_render._DebugRender._Wireframe = wireframe;
	debug_sphere_render._Sphere = sphere;
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

	//Update all debug axis aligned bounding box 3D renders.
	{
		SCOPED_LOCK(_DebugAxisAlignedBoundingBox3DRendersLock);

		for (uint64 i{ 0 }; i < _DebugAxisAlignedBoundingBox3DRenders.Size();)
		{
			_DebugAxisAlignedBoundingBox3DRenders[i]._CurrentLifetime += delta_time;

			if (_DebugAxisAlignedBoundingBox3DRenders[i]._CurrentLifetime >= _DebugAxisAlignedBoundingBox3DRenders[i]._MaximumLifetime)
			{
				_DebugAxisAlignedBoundingBox3DRenders.EraseAt<false>(i);
			}

			else
			{
				++i;
			}
		}
	}

	//Update all debug sphere renders.
	{
		SCOPED_LOCK(_DebugSphereRendersLock);

		for (uint64 i{ 0 }; i < _DebugSphereRenders.Size();)
		{
			_DebugSphereRenders[i]._CurrentLifetime += delta_time;

			if (_DebugSphereRenders[i]._CurrentLifetime >= _DebugSphereRenders[i]._MaximumLifetime)
			{
				_DebugSphereRenders.EraseAt<false>(i);
			}

			else
			{
				++i;
			}
		}
	}
}
#endif