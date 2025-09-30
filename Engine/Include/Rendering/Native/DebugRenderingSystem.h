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
	*	Debug rectangle render class definition.
	*/
	class DebugRectangleRender final
	{

	public:

		//The debug render.
		DebugRender _DebugRender;

		//The position.
		Vector3<float32> _Position;

		//The size.
		Vector2<float32> _Size;

		//The rotation.
		float32 _Rotation;

	};

	/*
	*	Initializes the debug rendering system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the debug rendering system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Debug renders a circle.
	*/
	void DebugRenderCircle
	(
		const Vector4<float32> &color,
		const float32 lifetime,
		const Vector3<float32> &position,
		const float32 radius
	) NOEXCEPT;

	/*
	*	Debug renders a rectangle.
	*/
	void DebugRenderRectangle
	(
		const Vector4<float32> &color,
		const float32 lifetime,
		const Vector3<float32> &position,
		const Vector2<float32> &size,
		const float32 rotation
	) NOEXCEPT;

private:

	//The debug circle renders lock.
	Spinlock _DebugCircleRendersLock;

	//Container for all debug circle renders.
	DynamicArray<DebugCircleRender> _DebugCircleRenders;

	//The debug rectangle renders lock.
	Spinlock _DebugRectangleRendersLock;

	//Container for all debug rectangle renders.
	DynamicArray<DebugRectangleRender> _DebugRectangleRenders;

	/*
	*	Gathers the debug render circle render input stream.
	*/
	void GatherDebugRenderCircleInputStream(class RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

	/*
	*	Gathers the debug render rectangle render input stream.
	*/
	void GatherDebugRenderRectangleInputStream(class RenderInputStream *const RESTRICT input_stream) NOEXCEPT;
	
};
#endif