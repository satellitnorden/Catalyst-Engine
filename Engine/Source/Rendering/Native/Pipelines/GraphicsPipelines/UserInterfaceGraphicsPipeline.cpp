//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/UserInterfaceMaterial.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElement.h>
#include <UserInterface/ImageUserInterfaceElement.h>
#include <UserInterface/TextUserInterfaceElement.h>
#include <UserInterface/UserInterfaceUtilities.h>

/*
*	User interface vertex push constant data definition.
*/
class UserInterfaceVertexPushConstantData final
{

public:

	//The minimum of the screen space axis aligned box.
	Vector2<float32> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float32> _Maximum;

};

/*
*	User interface fragment push constant data definition.
*/
class UserInterfaceFragmentPushConstantData final
{

public:

	//The type.
	uint32 _Type;

	//The element aspect ratio.
	float32 _ElementAspectRatio;

	//Padding.
	Padding<8> _Padding;

	//The material.
	UserInterfaceMaterial _Material;

};

/*
*	Initializes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::UserInterfaceVertex);
	SetTessellationControlShader(Shader::NONE);
	SetTessellationEvaluationShader(Shader::NONE);
	SetGeometryShader(Shader::NONE);
	SetFragmentShader(Shader::UserInterfaceFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(UserInterfaceVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetShouldClear(false);
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
	//Get the number of user interface elements.
	const uint64 number_of_user_interface_elements{ UserInterfaceSystem::Instance->GetUserInterfaceElements()->Size() };

	//If there's no to render, then... Don't.
	if (number_of_user_interface_elements == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data table.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Render all user interface elements.
	for (const UserInterfaceElement *const RESTRICT element : *UserInterfaceSystem::Instance->GetUserInterfaceElements())
	{
		//Render differently based on the type.
		switch (element->_Type)
		{
			case UserInterfaceElementType::BUTTON:
			{
				const ButtonUserInterfaceElement *const RESTRICT type_element{ static_cast<const ButtonUserInterfaceElement *const RESTRICT>(element) };

				//Push constants.
				UserInterfaceVertexPushConstantData vertex_data;

				vertex_data._Minimum = type_element->_Minimum;
				vertex_data._Maximum = type_element->_Maximum;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(UserInterfaceVertexPushConstantData), &vertex_data);

				UserInterfaceFragmentPushConstantData fragment_data;

				fragment_data._Type = static_cast<uint32>(UserInterfaceElementType::BUTTON);
				fragment_data._ElementAspectRatio = (type_element->_Maximum._X - type_element->_Minimum._X) / (type_element->_Maximum._Y - type_element->_Minimum._Y);
				
				switch (type_element->_CurrentState)
				{
					case ButtonUserInterfaceElement::State::IDLE:
					{
						fragment_data._Material = type_element->_IdleMaterial;

						break;
					}

					case ButtonUserInterfaceElement::State::HOVERED:
					{
						fragment_data._Material = type_element->_HoveredMaterial;

						break;
					}

					case ButtonUserInterfaceElement::State::PRESSED:
					{
						fragment_data._Material = type_element->_PressedMaterial;

						break;
					}
				}

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData), &fragment_data);

				//Draw!
				command_buffer->Draw(this, 4, 1);

				break;
			}

			case UserInterfaceElementType::IMAGE:
			{
				const ImageUserInterfaceElement *const RESTRICT type_element{ static_cast<const ImageUserInterfaceElement *const RESTRICT>(element) };

				//Push constants.
				UserInterfaceVertexPushConstantData vertex_data;

				vertex_data._Minimum = type_element->_Minimum;
				vertex_data._Maximum = type_element->_Maximum;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(UserInterfaceVertexPushConstantData), &vertex_data);

				UserInterfaceFragmentPushConstantData fragment_data;

				fragment_data._Type = static_cast<uint32>(UserInterfaceElementType::IMAGE);
				fragment_data._ElementAspectRatio = (type_element->_Maximum._X - type_element->_Minimum._X) / (type_element->_Maximum._Y - type_element->_Minimum._Y);
				fragment_data._Material = type_element->_Material;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData), &fragment_data);

				//Draw!
				command_buffer->Draw(this, 4, 1);

				break;
			}

			case UserInterfaceElementType::TEXT:
			{
				const TextUserInterfaceElement *const RESTRICT type_element{ static_cast<const TextUserInterfaceElement *const RESTRICT>(element) };

				//Calculate the aligned minimum/maximum.
				Vector2<float32> aligned_minimum;
				Vector2<float32> aligned_maximum;

				UserInterfaceUtilities::CalculateAlignedBoundingBox(type_element->_Minimum,
																	type_element->_Maximum,
																	type_element->_Font,
																	type_element->_Text,
																	type_element->_Scale,
																	type_element->_HorizontalAlignment,
																	type_element->_VerticalAlignment,
																	&aligned_minimum,
																	&aligned_maximum);

				//Draw all characters.
				float32 current_offset_X{ 0.0f };
				float32 current_offset_Y{ aligned_maximum._Y - aligned_minimum._Y - type_element->_Scale };

				for (uint64 i{ 0 }, length{ type_element->_Text.Length() }; i < length; ++i)
				{
					//Cache the chartacter.
					const char character{ type_element->_Text[i] };

					//Only draw if it´s a valid character.
					if (character != '\n')
					{
						//Push constants.
						UserInterfaceVertexPushConstantData vertex_data;

						vertex_data._Minimum._X = aligned_minimum._X + current_offset_X + type_element->_Font->_CharacterDescriptions[character]._Bearing._X * type_element->_Scale;
						vertex_data._Minimum._Y = aligned_minimum._Y + current_offset_Y - (type_element->_Font->_CharacterDescriptions[character]._Size._Y - type_element->_Font->_CharacterDescriptions[character]._Bearing._Y) * type_element->_Scale;

						vertex_data._Maximum._X = vertex_data._Minimum._X + type_element->_Font->_CharacterDescriptions[character]._Size._X * type_element->_Scale;
						vertex_data._Maximum._Y = vertex_data._Minimum._Y + type_element->_Font->_CharacterDescriptions[character]._Size._Y * type_element->_Scale;

						command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(UserInterfaceVertexPushConstantData), &vertex_data);

						UserInterfaceFragmentPushConstantData fragment_data;

						fragment_data._Type = static_cast<uint32>(UserInterfaceElementType::TEXT);
						fragment_data._ElementAspectRatio = (aligned_maximum._X - aligned_minimum._X) / (aligned_maximum._Y - aligned_minimum._Y);
						fragment_data._Material.SetPrimaryTextureIndex(type_element->_Font->_CharacterDescriptions[character]._TextureIndex);

						command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData), &fragment_data);

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

						for (uint64 j{ i + 1 }; j < length && type_element->_Text[j] != ' '; ++j)
						{
							temporary_offset_X += type_element->_Font->_CharacterDescriptions[type_element->_Text[j]]._Advance * type_element->_Scale;

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
						current_offset_Y -= type_element->_Scale;
					}

					else
					{
						current_offset_X += type_element->_Font->_CharacterDescriptions[character]._Advance * type_element->_Scale;
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