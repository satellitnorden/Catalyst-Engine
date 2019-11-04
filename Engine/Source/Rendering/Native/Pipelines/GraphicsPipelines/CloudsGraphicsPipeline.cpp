//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/CloudsGraphicsPipeline.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Texture3D.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Create the cloud texture.
	//CreateCloudTexture();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::CloudsFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::NotEqual);
	SetStencilCompareMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);


	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the cloud texture.
*/
void CloudsGraphicsPipeline::CreateCloudTexture() NOEXCEPT
{
	//Defone constants.
	constexpr uint32 CLOUD_TEXTURE_RESOLUTION{ 64 };

	//Create the texture 3d.
	Texture3D<byte> texture{ CLOUD_TEXTURE_RESOLUTION };

	for (uint32 X{ 0 }; X < CLOUD_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < CLOUD_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 Z{ 0 }; Z < CLOUD_TEXTURE_RESOLUTION; ++Z)
			{
				texture.At(X, Y, Z) = CatalystRandomMath::RandomIntegerInRange<byte>(0, 255);
			}
		}
	}

	//Create the texture 3D.
	RenderingSystem::Instance->CreateTexture3D(	TextureData(TextureDataContainer(texture),
															TextureFormat::R8_Byte), &_CloudTexture);
}