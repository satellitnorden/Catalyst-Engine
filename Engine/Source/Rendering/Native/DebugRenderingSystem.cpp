#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/DebugRenderingSystem.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Debug render circle push constant data class definition.
class DebugRenderCirclePushConstantData final
{

public:

	//The position/radius.
	Vector4<float32> _PositionRadius;

	//The color.
	Vector4<float32> _Color;

};

//Debug render rectangle push constant data class definition.
class DebugRenderRectanglePushConstantData final
{

public:

	//The position/rotation.
	Vector4<float32> _PositionRotation;

	//The color.
	Vector4<float32> _Color;

	//The size.
	Vector2<float32> _Size;

};

/*
*	Initializes the debug rendering system.
*/
void DebugRenderingSystem::Initialize() NOEXCEPT
{
	//Register the "DebugRenderCircle" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("DebugRenderCircle"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(DebugRenderCirclePushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			RenderingSystem::Instance->GetDebugRenderingSystem()->GatherDebugRenderCircleInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);

	//Register the "DebugRenderRectangle" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("DebugRenderRectangle"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(DebugRenderRectanglePushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			RenderingSystem::Instance->GetDebugRenderingSystem()->GatherDebugRenderRectangleInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);
}

/*
*	Updates the debug rendering system.
*/
void DebugRenderingSystem::Update() NOEXCEPT
{
	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update all debug circle renders.
	{
		SCOPED_LOCK(_DebugCircleRendersLock);

		for (uint64 i{ 0 }; i < _DebugCircleRenders.Size();)
		{
			_DebugCircleRenders[i]._DebugRender._CurrentLifetime += delta_time;

			if (_DebugCircleRenders[i]._DebugRender._CurrentLifetime >= _DebugCircleRenders[i]._DebugRender._MaximumLifetime)
			{
				_DebugCircleRenders.EraseAt<false>(i);
			}

			else
			{
				++i;
			}
		}
	}

	//Update all debug rectangle renders.
	{
		SCOPED_LOCK(_DebugRectangleRendersLock);

		for (uint64 i{ 0 }; i < _DebugRectangleRenders.Size();)
		{
			_DebugRectangleRenders[i]._DebugRender._CurrentLifetime += delta_time;

			if (_DebugRectangleRenders[i]._DebugRender._CurrentLifetime >= _DebugRectangleRenders[i]._DebugRender._MaximumLifetime)
			{
				_DebugRectangleRenders.EraseAt<false>(i);
			}

			else
			{
				++i;
			}
		}
	}
}

/*
*	Debug renders a circle.
*/

void DebugRenderingSystem::DebugRenderCircle
(
	const Vector4<float32> &color,
	const float32 lifetime,
	const Vector3<float32> &position,
	const float32 radius
) NOEXCEPT
{
	//Add the debug circle render.
	DebugCircleRender debug_circle_render;

	debug_circle_render._DebugRender._Color = color;
	debug_circle_render._DebugRender._CurrentLifetime = 0.0f;
	debug_circle_render._DebugRender._MaximumLifetime = lifetime;
	debug_circle_render._Position = position;
	debug_circle_render._Radius = radius;

	{
		SCOPED_LOCK(_DebugCircleRendersLock);

		_DebugCircleRenders.Emplace(debug_circle_render);
	}
}

/*
*	Debug renders a rectangle.
*/
void DebugRenderingSystem::DebugRenderRectangle
(
	const Vector4<float32> &color,
	const float32 lifetime,
	const Vector3<float32> &position,
	const Vector2<float32> &size,
	const float32 rotation
) NOEXCEPT
{
	//Add the debug rectangle render.
	DebugRectangleRender debug_rectangle_render;

	debug_rectangle_render._DebugRender._Color = color;
	debug_rectangle_render._DebugRender._CurrentLifetime = 0.0f;
	debug_rectangle_render._DebugRender._MaximumLifetime = lifetime;
	debug_rectangle_render._Position = position;
	debug_rectangle_render._Size = size;
	debug_rectangle_render._Rotation = rotation;

	{
		SCOPED_LOCK(_DebugRectangleRendersLock);

		_DebugRectangleRenders.Emplace(debug_rectangle_render);
	}
}

/*
*	Gathers the debug render circle render input stream.
*/
void DebugRenderingSystem::GatherDebugRenderCircleInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Go through all debug circle renders.
	{
		SCOPED_LOCK(_DebugCircleRendersLock);

		for (const DebugCircleRender &debug_render : _DebugCircleRenders)
		{
			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = 0;
			
			//Set up the push constant data.
			DebugRenderCirclePushConstantData push_constant_data;

			push_constant_data._PositionRadius = Vector4<float32>(debug_render._Position, debug_render._Radius);
			push_constant_data._Color = debug_render._DebugRender._Color;

			for (uint64 i{ 0 }; i < sizeof(DebugRenderCirclePushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}

/*
*	Gathers the debug render rectangle render input stream.
*/
void DebugRenderingSystem::GatherDebugRenderRectangleInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Go through all debug rectangle renders.
	{
		SCOPED_LOCK(_DebugRectangleRendersLock);

		for (const DebugRectangleRender &debug_render : _DebugRectangleRenders)
		{
			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = 0;

			//Set up the push constant data.
			DebugRenderRectanglePushConstantData push_constant_data;

			push_constant_data._PositionRotation = Vector4<float32>(debug_render._Position, debug_render._Rotation);
			push_constant_data._Color = debug_render._DebugRender._Color;
			push_constant_data._Size = debug_render._Size;

			for (uint64 i{ 0 }; i < sizeof(DebugRenderRectanglePushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}
#endif