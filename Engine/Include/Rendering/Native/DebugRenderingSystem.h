#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Math.
#include <Math/General/Vector.h>

class DebugRenderingSystem final
{

public:

	/*
	*	Debug render class definition.
	*/
	class DebugRender final
	{

	public:

		//The color.
		Vector4<float32> _Color;

		//The current lifetime.
		float32 _CurrentLifetime;

		//The maximum lifetime.
		float32 _MaximumLifetime;

		//Whether or not to perform depth test.
		bool _DepthTest;

	};

	/*
	*	Debug circle render class definition.
	*/
	class DebugCircleRender final
	{

	public:

		//The debug render.
		DebugRender _DebugRender;

		//The position.
		Vector3<float32> _Position;

		//The radius.
		float32 _Radius;

	};

	/*
	*	Initializes the debug rendering system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Debug renders an axis aligned bounding box 3D.
	*/
	void DebugRenderCircle
	(
		const Vector4<float32>& color,
		const float32 lifetime,
		const bool depth_test,
		const Vector3<float32>& position,
		const float32 radius
	) NOEXCEPT;

private:

	//The debug circle renders lock.
	Spinlock _DebugCircleRendersLock;

	//Container for all debug circle renders.
	DynamicArray<DebugCircleRender> _DebugCircleRenders;

	/*
	*	Gathers the debug render circle render input stream.
	*/
	void GatherDebugRenderCircleInputStream(class RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

	/*
	*	Updates the debug rendering system during the post update phase.
	*/
	void PostUpdate() NOEXCEPT;
	
};
#endif