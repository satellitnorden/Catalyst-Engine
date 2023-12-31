//Header file.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

/*
*	Resets this pipeline.
*/
void Pipeline::ResetPipeline() NOEXCEPT
{
	//Clear the render data table layouts.
	_RenderDataTableLayouts.Clear();

	//Clear the push constant ranges.
	_PushConstantRanges.Clear();
}

/*
*	Processes the given input stream.
*/
void Pipeline::ProcessInputStream(const RenderInputStream &input_stream, CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	switch (input_stream._Mode)
	{
		case RenderInputStream::Mode::DISPATCH:
		{
			for (const RenderInputStreamEntry &entry : input_stream._Entries)
			{
				//Push constants.
				if (input_stream._RequiredPushConstantDataSize != 0)
				{
					command_buffer->PushConstants
					(
						this,
						ShaderStage::COMPUTE,
						0,
						static_cast<uint32>(input_stream._RequiredPushConstantDataSize),
						&input_stream._PushConstantDataMemory[entry._PushConstantDataOffset]
					);
				}

				//Dispatch!
				command_buffer->Dispatch(this, entry._DispatchWidth, entry._DispatchHeight, entry._DispatchDepth);
			}

			break;
		}

		case RenderInputStream::Mode::DRAW:
		{
			for (const RenderInputStreamEntry& entry : input_stream._Entries)
			{
				//Push constants.
				if (input_stream._RequiredPushConstantDataSize != 0)
				{
					command_buffer->PushConstants
					(
						this,
						ShaderStage::VERTEX | ShaderStage::FRAGMENT,
						0,
						static_cast<uint32>(input_stream._RequiredPushConstantDataSize),
						&input_stream._PushConstantDataMemory[entry._PushConstantDataOffset]
					);
				}

				//Draw!
				command_buffer->Draw(this, entry._VertexCount, 1);
			}

			break;
		}

		case RenderInputStream::Mode::DRAW_INSTANCED:
		{
			for (const RenderInputStreamEntry& entry : input_stream._Entries)
			{
				command_buffer->PushConstants
				(
					this,
					ShaderStage::VERTEX | ShaderStage::FRAGMENT,
					0,
					static_cast<uint32>(input_stream._RequiredPushConstantDataSize),
					&input_stream._PushConstantDataMemory[entry._PushConstantDataOffset]
				);

				command_buffer->BindVertexBuffer(this, 0, entry._InstanceBuffer, &OFFSET);

				command_buffer->Draw(this, entry._VertexCount, entry._InstanceCount);
			}

			break;
		}

		case RenderInputStream::Mode::DRAW_INDEXED:
		{
			for (const RenderInputStreamEntry& entry : input_stream._Entries)
			{
				command_buffer->PushConstants
				(
					this,
					ShaderStage::VERTEX | ShaderStage::FRAGMENT,
					0,
					static_cast<uint32>(input_stream._RequiredPushConstantDataSize),
					&input_stream._PushConstantDataMemory[entry._PushConstantDataOffset]
				);

				command_buffer->BindVertexBuffer(this, 0, entry._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, entry._IndexBuffer, entry._IndexBufferOffset);

				command_buffer->DrawIndexed(this, entry._IndexCount, 1);
			}

			break;
		}

		case RenderInputStream::Mode::DRAW_INDEXED_INSTANCED:
		{
			for (const RenderInputStreamEntry& entry : input_stream._Entries)
			{
				command_buffer->PushConstants
				(
					this,
					ShaderStage::VERTEX | ShaderStage::FRAGMENT,
					0,
					static_cast<uint32>(input_stream._RequiredPushConstantDataSize),
					&input_stream._PushConstantDataMemory[entry._PushConstantDataOffset]
				);

				command_buffer->BindVertexBuffer(this, 0, entry._VertexBuffer, &OFFSET);
				command_buffer->BindVertexBuffer(this, 1, entry._InstanceBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, entry._IndexBuffer, entry._IndexBufferOffset);

				command_buffer->DrawIndexed(this, entry._IndexCount, entry._InstanceCount);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}