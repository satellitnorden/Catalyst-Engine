//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElement.h>
#include <UserInterface/TextUserInterfaceElement.h>

/*
*	User interface vertex push constant data definition.
*/
class UserInterfaceVertexPushConstantData final
{

public:

	//The minimum of the screen space axis aligned box.
	Vector2<float> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float> _Maximum;

};

/*
*	User interface fragment push constant data definition.
*/
class UserInterfaceFragmentPushConstantData final
{

public:

	//The type.
	int32 _Type;

	//The texture index.
	int32 _TextureIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::UserInterfaceVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::UserInterfaceFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(UserInterfaceVertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
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
	const uint64 numberOfUserInterfaceElements{ ComponentManager::ReadSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements.Size() };

	//If there's no to render, then... Don't.
	if (numberOfUserInterfaceElements == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Render all user interface elements.
	for (const UserInterfaceElement *const RESTRICT element : ComponentManager::ReadSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements)
	{
		//Render differently based on the type.
		switch (element->_Type)
		{
			case UserInterfaceElementType::Image:
			{
				const ImageUserInterfaceElement *const RESTRICT typeElement{ static_cast<const ImageUserInterfaceElement *const RESTRICT>(element) };

				//Push constants.
				UserInterfaceVertexPushConstantData vertexData;

				vertexData._Minimum = typeElement->_Minimum;
				vertexData._Maximum = typeElement->_Maximum;

				commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(UserInterfaceVertexPushConstantData), &vertexData);

				UserInterfaceFragmentPushConstantData fragmentData;

				fragmentData._Type = static_cast<int32>(UserInterfaceElementType::Image);
				fragmentData._TextureIndex = typeElement->_TextureIndex;

				commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData), &fragmentData);

				//Draw!
				commandBuffer->Draw(this, 4, 1);

				break;
			}

			case UserInterfaceElementType::Text:
			{
				constexpr float SCALE{ 0.03f };

				const TextUserInterfaceElement *const RESTRICT typeElement{ static_cast<const TextUserInterfaceElement *const RESTRICT>(element) };

				//Draw all characters.
				float currentOffsetX{ 0.0f };
				float currentOffsetY{ typeElement->_Maximum._Y - typeElement->_Minimum._Y - SCALE };

				for (uint64 i{ 0 }, length{ typeElement->_Text.Length() }; i < length; ++i)
				{
					//Cache the chartacter.
					const char character{ typeElement->_Text[i] };

					//Push constants.
					UserInterfaceVertexPushConstantData vertexData;

					vertexData._Minimum._X = typeElement->_Minimum._X + currentOffsetX + typeElement->_Font->_CharacterDescriptions[character]._Bearing._X * SCALE;
					vertexData._Minimum._Y = typeElement->_Minimum._Y + currentOffsetY - (typeElement->_Font->_CharacterDescriptions[character]._Size._Y - typeElement->_Font->_CharacterDescriptions[character]._Bearing._Y) * SCALE;

					vertexData._Maximum._X = vertexData._Minimum._X + typeElement->_Font->_CharacterDescriptions[character]._Size._X * SCALE;
					vertexData._Maximum._Y = vertexData._Minimum._Y + typeElement->_Font->_CharacterDescriptions[character]._Size._Y * SCALE;

					commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(UserInterfaceVertexPushConstantData), &vertexData);

					UserInterfaceFragmentPushConstantData fragmentData;

					fragmentData._Type = static_cast<int32>(UserInterfaceElementType::Text);
					fragmentData._TextureIndex = typeElement->_Font->_CharacterDescriptions[character]._TextureIndex;

					commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(UserInterfaceVertexPushConstantData), sizeof(UserInterfaceFragmentPushConstantData), &fragmentData);

					//Draw!
					commandBuffer->Draw(this, 4, 1);

					//If this character is a space, look ahead toward the next space to see if the line should wrap around.
					if (character == ' ')
					{
						bool shouldWrapAround{ false };

						float temporaryOffsetX{ currentOffsetX };

						for (uint64 j{ i + 1 }; j < length && typeElement->_Text[j] != ' '; ++j)
						{
							temporaryOffsetX += typeElement->_Font->_CharacterDescriptions[typeElement->_Text[j]]._Advance * SCALE;

							if (temporaryOffsetX >= typeElement->_Maximum._X - typeElement->_Minimum._X)
							{
								shouldWrapAround = true;

								break;
							}
						}

						if (shouldWrapAround)
						{
							currentOffsetX = 0.0f;
							currentOffsetY -= SCALE;
						}

						else
						{
							currentOffsetX += typeElement->_Font->_CharacterDescriptions[character]._Advance * SCALE;
						}
					}
					
					else
					{
						currentOffsetX += typeElement->_Font->_CharacterDescriptions[character]._Advance * SCALE;
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
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}