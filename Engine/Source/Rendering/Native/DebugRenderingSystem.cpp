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

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void *const RESTRICT arguments)
	{
		static_cast<DebugRenderingSystem *const RESTRICT>(arguments)->PostUpdate();
	},
	this,
	UpdatePhase::POST,
	UpdatePhase::PRE,
	false,
	false);
}

/*
*	Debug renders an axis aligned bounding box 3D.
*/

void DebugRenderingSystem::DebugRenderCircle
(
	const Vector4<float32> &color,
	const float32 lifetime,
	const bool depth_test,
	const Vector3<float32> &position,
	const float32 radius
) NOEXCEPT
{
	//Add the debug circle render.
	DebugCircleRender debug_circle_render;

	debug_circle_render._DebugRender._Color = color;
	debug_circle_render._DebugRender._CurrentLifetime = 0.0f;
	debug_circle_render._DebugRender._MaximumLifetime = lifetime;
	debug_circle_render._DebugRender._DepthTest = depth_test;
	debug_circle_render._Position = position;
	debug_circle_render._Radius = radius;

	{
		SCOPED_LOCK(_DebugCircleRendersLock);

		_DebugCircleRenders.Emplace(debug_circle_render);
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

	//Go through all debug circler renders.
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
*	Updates the debug rendering system during the post update phase.
*/
void DebugRenderingSystem::PostUpdate() NOEXCEPT
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
}
#endif