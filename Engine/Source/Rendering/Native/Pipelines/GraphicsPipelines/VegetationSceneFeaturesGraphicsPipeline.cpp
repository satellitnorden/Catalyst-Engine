//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Vegetation vertex push constant data definition.
*/
class VegetationVertexPushConstantData final
{

public:

	float _LargeScaleWindDisplacementFactor;
	float _MediumScaleWindDisplacementFactor;
	float _SmallScaleWindDisplacementFactor;

};

/*
*	Vegetation fragment push constant data definition.
*/
class VegetationFragmentPushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;
	int32 _MaterialPropertiesTextureIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void VegetationSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::VegetationColorSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::VegetationColorSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(5);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Velocity));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VegetationVertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VegetationVertexPushConstantData), sizeof(VegetationFragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(8);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(Vertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	4,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[0]));
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[1]));
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[2]));
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[3]));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(Vertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);
}

/*
*	Executes this graphics pipeline.
*/
void VegetationSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all vegetation components and draw them all.
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there's none to render - render none.
	if (numberOfVegetationComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationVegetationComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for vegetation culling to finish.
	CullingSystem::Instance->WaitForVegetationCulling();

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		//Don't draw if it's not visible.
		if (!component->_Visibility)
		{
			continue;
		}

		//Don't draw if it's not an opaque material.
		if (component->_Material._Type != Material::Type::Opaque)
		{
			continue;
		}

		//Push constants.
		VegetationVertexPushConstantData vertex_data;

		vertex_data._LargeScaleWindDisplacementFactor = component->_LargeScaleWindDisplacementFactor;
		vertex_data._MediumScaleWindDisplacementFactor = component->_MediumScaleWindDisplacementFactor;
		vertex_data._SmallScaleWindDisplacementFactor = component->_SmallScaleWindDisplacementFactor;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VegetationVertexPushConstantData), &vertex_data);

		VegetationFragmentPushConstantData fragment_data;

		fragment_data._AlbedoTextureIndex = component->_Material._AlbedoTextureIndex;
		fragment_data._NormalMapTextureIndex = component->_Material._NormalMapTextureIndex;
		fragment_data._MaterialPropertiesTextureIndex = component->_Material._MaterialPropertiesTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VegetationVertexPushConstantData), sizeof(VegetationFragmentPushConstantData), &fragment_data);

		//Bind the vertex/inder buffer.
		commandBuffer->BindVertexBuffer(this, 0, component->_Model->_VertexBuffer, &OFFSET);
		commandBuffer->BindIndexBuffer(this, component->_Model->_IndexBuffer, OFFSET);

		//Bind the transformations buffer.
		commandBuffer->BindVertexBuffer(this, 1, component->_TransformationsBuffer, &OFFSET);

		commandBuffer->DrawIndexed(this, component->_Model->_IndexCount, component->_NumberOfTransformations);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}