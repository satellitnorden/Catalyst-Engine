//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/UserInterfaceMaterial.h>
#include <Rendering/Native/UserInterfaceUniformData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/UserInterfaceUtilities.h>

/*
*	User interface scene features push constant data definition.
*/
class UserInterfaceSceneFeaturesPushConstantData final
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
void UserInterfaceSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ThreeDimensionalUserInterfaceVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ThreeDimensionalUserInterfaceFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(5);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::USER_INTERFACE));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfaceSceneFeaturesPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetFullResolution());

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void UserInterfaceSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache relevant data.
	const uint64 number_of_components{ ComponentManager::GetNumberOfUserInterfaceComponents() };
	UserInterfaceComponent *RESTRICT component{ ComponentManager::GetUserInterfaceUserInterfaceComponents() };

	//If there's none to render, then... Don't.
	if (number_of_components == 0)
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

	//Render all user interface entities.
	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		//Skip if this component doesn't have a user interface scene yet.
		if (!component->_UserInterfaceScene)
		{
			continue;
		}

		//Update the current uniform buffer.
		{
			BufferHandle &current_uniform_buffer{ component->_UniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

			UserInterfaceUniformData uniform_data;

			uniform_data._ToWorldMatrix = Matrix4x4(component->_WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()), component->_Rotation, VectorConstants::ONE);
			uniform_data._Normal = CatalystCoordinateSpacesUtilities::RotatedWorldBackwardVector(component->_Rotation);
			uniform_data._Scale = component->_Scale;
			uniform_data._Roughness = component->_Roughness;
			uniform_data._Metallic = component->_Metallic;
			uniform_data._AmbientOcclusion = component->_AmbientOcclusion;
			uniform_data._EmissiveMultiplier = component->_EmissiveMultiplier;

			{
				const void *const RESTRICT data_chunks[]{ &uniform_data };
				const uint64 data_sizes[]{ sizeof(UserInterfaceUniformData) };

				RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_uniform_buffer);
			}
		}

		//Bind the render data table.
		command_buffer->BindRenderDataTable(this, 1, component->_RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()]);

		//Retrieve the primitives for the given scene.
		DynamicArray<const UserInterfacePrimitive *RESTRICT> primitives;

		component->_UserInterfaceScene->RetrieveUserInterfacePrimitives(&primitives);

		//Render all user interface primitives.
		for (const UserInterfacePrimitive *const RESTRICT primitive : primitives)
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
					UserInterfaceSceneFeaturesPushConstantData data;

					data._Material = type_primitive->_Material;
					data._Color = Vector4<float32>(1.0f, 1.0f, 1.0f, primitive->_Opacity);
					data._Minimum = type_primitive->_Minimum;
					data._Maximum = type_primitive->_Maximum;
					data._Type = static_cast<uint32>(UserInterfacePrimitiveType::IMAGE);
					data._WidthRangeStart = 0.0f;
					data._WidthRangeEnd = 1.0f;
					data._PrimitiveAspectRatio = (type_primitive->_Maximum._X - type_primitive->_Minimum._X) / (type_primitive->_Maximum._Y - type_primitive->_Minimum._Y);
					data._TextSmoothingFactor = 0.0f;

					command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfaceSceneFeaturesPushConstantData), &data);

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
							UserInterfaceSceneFeaturesPushConstantData data;

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

							command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(UserInterfaceSceneFeaturesPushConstantData), &data);

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
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void UserInterfaceSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}