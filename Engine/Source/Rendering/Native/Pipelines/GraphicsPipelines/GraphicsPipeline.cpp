//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

/*
*	Default constructor.
*/
GraphicsPipeline::GraphicsPipeline() NOEXCEPT
{
	//Set the type.
	SetType(Type::GRAPHICS);
}

/*
*	Resetis this graphics pipeline.
*/
void GraphicsPipeline::ResetGraphicsPipeline() NOEXCEPT
{	
	//Reset this pipeline.
	ResetPipeline();

	//Clear the output render targets.
	_OutputRenderTargets.Clear();

	//Clear the vertex input attribute descriptions.
	_VertexInputAttributeDescriptions.Clear();

	//Clear the vertex input binding descriptions.
	_VertexInputBindingDescriptions.Clear();
}

/*
*	Processes the given input stream.
*/
void GraphicsPipeline::ProcessInputStream(const RenderInputStream &input_stream, CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	switch (input_stream._Mode)
	{
		case RenderInputStream::Mode::DRAW:
		{
			for (const RenderInputStreamEntry &entry : input_stream._Entries)
			{
				//Draw!
				command_buffer->Draw(this, entry._VertexCount, 1);
			}

			break;
		}

		case RenderInputStream::Mode::DRAW_INSTANCED:
		{
			for (const RenderInputStreamEntry &entry : input_stream._Entries)
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
			for (const RenderInputStreamEntry &entry : input_stream._Entries)
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
			for (const RenderInputStreamEntry &entry : input_stream._Entries)
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