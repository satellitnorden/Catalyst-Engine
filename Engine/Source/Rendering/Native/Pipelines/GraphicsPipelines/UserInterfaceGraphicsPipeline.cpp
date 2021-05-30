//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/UserInterfaceMaterial.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/UserInterfaceUtilities.h>

/*
*	User interface push constant data definition.
*/
class UserInterfacePushConstantData final
{

public:

	//The material.
	UserInterfaceMaterial _Material;

	//The color.
	Vector4<float32> _Color;

	//The minimum of the screen space axis aligned box.
	Vector2<float32> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float32> _Maximum;

	//The type.
	uint32 _Type;

	//The width range start.
	float32 _WidthRangeStart;

	//The width range end.
	float32 _WidthRangeEnd;

	//The primitive aspect ratio.
	float32 _PrimitiveAspectRatio;

	//The text smoothing factor.
	float32 _TextSmoothingFactor;

};

/*
*	Initializes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("UserInterfaceVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("UserInterfaceFragmentShader")));

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfacePushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Execute() NOEXCEPT
{
	//Get the number of user interface primitives.
	const uint64 number_of_user_interface_primitives{ UserInterfaceSystem::Instance->GetUserInterfacePrimitives()->Size() };

	//If there's none to render, then... Don't.
	if (number_of_user_interface_primitives == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);;

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data table.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Render all user interface primitives.
	for (const UserInterfacePrimitive *const RESTRICT primitive : *UserInterfaceSystem::Instance->GetUserInterfacePrimitives())
	{
		//Don't render this if it isn't visible. (:
		if (primitive->_Opacity <= 0.0f)
		{
			continue;
		}

		//Render differently based on the type.
		switch (primitive->_Type)
		{
			case UserInterfacePrimitiveType::IMAGE:
			{
				const ImageUserInterfacePrimitive *const RESTRICT type_primitive{ static_cast<const ImageUserInterfacePrimitive *const RESTRICT>(primitive) };

				//Push constants.
				UserInterfacePushConstantData data;

				data._Material = type_primitive->_Material;
				data._Color = Vector4<float32>(1.0f, 1.0f, 1.0f, primitive->_Opacity);
				data._Minimum = type_primitive->_Minimum;
				data._Maximum = type_primitive->_Maximum;
				data._Type = static_cast<uint32>(UserInterfacePrimitiveType::IMAGE);
				data._WidthRangeStart = 0.0f;
				data._WidthRangeEnd = 1.0f;
				data._PrimitiveAspectRatio = (type_primitive->_Maximum._X - type_primitive->_Minimum._X) / (type_primitive->_Maximum._Y - type_primitive->_Minimum._Y);
				data._TextSmoothingFactor = 0.0f;

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfacePushConstantData), &data);

				//Draw!
				command_buffer->Draw(this, 4, 1);

				break;
			}

			case UserInterfacePrimitiveType::TEXT:
			{
				const TextUserInterfacePrimitive *const RESTRICT type_primitive{ static_cast<const TextUserInterfacePrimitive *const RESTRICT>(primitive) };

				//Calculate the aligned minimum/maximum.
				Vector2<float32> aligned_minimum;
				Vector2<float32> aligned_maximum;

				UserInterfaceUtilities::CalculateAlignedBoundingBox(type_primitive->_Minimum,
																	type_primitive->_Maximum,
																	type_primitive->_FontResource,
																	type_primitive->_Text,
																	type_primitive->_Scale,
																	type_primitive->_HorizontalAlignment,
																	type_primitive->_VerticalAlignment,
																	&aligned_minimum,
																	&aligned_maximum);

				//Draw all characters.
				float32 current_offset_X{ 0.0f };
				float32 current_offset_Y{ aligned_maximum._Y - aligned_minimum._Y - type_primitive->_Scale };

				for (uint64 i{ 0 }, length{ type_primitive->_Text.Length() }; i < length; ++i)
				{
					//Cache the chartacter.
					const char character{ type_primitive->_Text[i] };

					//Only draw if it´s a valid character.
					if (character != '\n')
					{
						//Push constants.
						UserInterfacePushConstantData data;

						data._Material.SetPrimaryTextureIndex(type_primitive->_FontResource->_MasterTextureIndex);
						data._Color = Vector4<float32>(1.0f, 1.0f, 1.0f, type_primitive->_Opacity);
						data._Minimum._X = aligned_minimum._X + current_offset_X + type_primitive->_FontResource->_CharacterDescriptions[character]._Bearing._X * type_primitive->_Scale;
						data._Minimum._Y = aligned_minimum._Y + current_offset_Y - (type_primitive->_FontResource->_CharacterDescriptions[character]._Size._Y - type_primitive->_FontResource->_CharacterDescriptions[character]._Bearing._Y) * type_primitive->_Scale;
						data._Maximum._X = data._Minimum._X + type_primitive->_FontResource->_CharacterDescriptions[character]._Size._X * type_primitive->_Scale;
						data._Maximum._Y = data._Minimum._Y + type_primitive->_FontResource->_CharacterDescriptions[character]._Size._Y * type_primitive->_Scale;
						data._Type = static_cast<uint32>(UserInterfacePrimitiveType::TEXT);
						data._WidthRangeStart = type_primitive->_FontResource->_CharacterDescriptions[character]._TextureWidthOffsetStart;
						data._WidthRangeEnd = type_primitive->_FontResource->_CharacterDescriptions[character]._TextureWidthOffsetEnd;
						data._PrimitiveAspectRatio = (aligned_maximum._X - aligned_minimum._X) / (aligned_maximum._Y - aligned_minimum._Y);
						data._TextSmoothingFactor = type_primitive->_TextSmoothingFactor;

						command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfacePushConstantData), &data);

						//Draw!
						command_buffer->Draw(this, 4, 1);
					}

					//Should the text wrap around?
					bool should_wrap_around{ false };

					//It should wrap around if a line break is specified.
					if (character == '\n')
					{
						should_wrap_around = true;
					}

					//If this character is a space, look ahead toward the next space to see if the line should wrap around.
					else if (character == ' ')
					{
						float32 temporary_offset_X{ current_offset_X };

						for (uint64 j{ i + 1 }; j < length && type_primitive->_Text[j] != ' '; ++j)
						{
							temporary_offset_X += type_primitive->_FontResource->_CharacterDescriptions[type_primitive->_Text[j]]._Advance * type_primitive->_Scale;

							if (temporary_offset_X >= aligned_maximum._X - aligned_minimum._X)
							{
								should_wrap_around = true;

								break;
							}
						}
					}
					
					//Perform the wrap around, if necessary.
					if (should_wrap_around)
					{
						current_offset_X = 0.0f;
						current_offset_Y -= type_primitive->_Scale;
					}

					else
					{
						current_offset_X += type_primitive->_FontResource->_CharacterDescriptions[character]._Advance * type_primitive->_Scale;
					}
				}

				break;
			}

			default:
			{
				ASSERT(false, "Unhandled case!");

				break;
			}
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}