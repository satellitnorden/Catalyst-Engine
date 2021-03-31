//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ProceduralGrassSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Procedural grass push constant data definition.
*/
class ProceduralGrassPushConstantData final
{

public:

	//The resolution.
	Vector2<uint32> _Resolution;

	//The resolution reciprocal.
	Vector2<float32> _ResolutionReciprocal;

	//The distance between blades.
	float32 _DistanceBetweenBlades;

};

/*
*	Initializes this graphics pipeline.
*/
void ProceduralGrassSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ProceduralGrassSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourceSystem::Instance->GetShaderResource(HashString("ProceduralGrassSceneFeaturesGeometryShader")));
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ProceduralGrassSceneFeaturesFragmentShader")));

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
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::GEOMETRY | ShaderStage::FRAGMENT, 0, sizeof(ProceduralGrassPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::PointList);
}

/*
*	Executes this graphics pipeline.
*/
void ProceduralGrassSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//If procedural grass isn't enabled, just return.
	if (!WorldSystem::Instance->GetVegetationSystem()->IsProceduralGrassEnabled())
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache the procedural grass properties.
	const ProceduralGrassProperties &procedural_grass_properties{ WorldSystem::Instance->GetVegetationSystem()->GetProceduralGrassProperties() };

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Push constants.
	ProceduralGrassPushConstantData data;

	const float32 aspect_ratio{ static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Width) / static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Height) };
	data._Resolution._X = CatalystBaseMath::Round<uint32>(procedural_grass_properties._ScreenCoverage * aspect_ratio);
	data._Resolution._Y = CatalystBaseMath::Round<uint32>(procedural_grass_properties._ScreenCoverage);
	data._ResolutionReciprocal._X = 1.0f / static_cast<float32>(data._Resolution._X);
	data._ResolutionReciprocal._Y = 1.0f / static_cast<float32>(data._Resolution._Y);
	data._DistanceBetweenBlades = procedural_grass_properties._DistanceBetweenBlades;

	command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::GEOMETRY | ShaderStage::FRAGMENT, 0, sizeof(ProceduralGrassPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, data._Resolution._X * data._Resolution._Y, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}