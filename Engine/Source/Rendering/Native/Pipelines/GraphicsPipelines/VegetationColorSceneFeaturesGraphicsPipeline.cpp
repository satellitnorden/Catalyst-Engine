//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationColorSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

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

	int32 _GlobalMaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void VegetationColorSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("VegetationColorSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("VegetationColorSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(4);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(VegetationVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(VegetationVertexPushConstantData), sizeof(VegetationFragmentPushConstantData));

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
										offsetof(Matrix4x4, _Matrix[0]));
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[1]));
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[2]));
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[3]));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(Vertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4x4), VertexInputBindingDescription::InputRate::Instance);

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
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
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
void VegetationColorSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Cache data the will be used.
	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationVegetationComponents() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for vegetation culling and level of detail to finish.
	CullingSystem::Instance->WaitForVegetationCulling();
	LevelOfDetailSystem::Instance->WaitForVegetationLevelOfDetail();

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		//Don't draw if it's not visible.
		if (!component->_Visibility)
		{
			continue;
		}

		//Don't draw if it's not the correct level of detail.
		if (component->_LevelOfDetail != VegetationComponent::LevelOfDetail::Full)
		{
			continue;
		}

		//Don't draw if it's not a masked material.
		if (RenderingSystem::Instance->GetMaterialSystem()->GetGlobalMaterial(component->_GlobalMaterialIndex)._Type != Material::Type::Masked)
		{
			continue;
		}

		//Draw all meshes.
		for (uint64 i{ 0 }, size{ component->_Model->_Meshes.Size() }; i < size; ++i)
		{
			//Cache the mesh.
			const Mesh& mesh{ component->_Model->_Meshes[i] };

			//Push constants.
			VegetationVertexPushConstantData vertex_data;

			vertex_data._LargeScaleWindDisplacementFactor = component->_LargeScaleWindDisplacementFactor;
			vertex_data._MediumScaleWindDisplacementFactor = component->_MediumScaleWindDisplacementFactor;
			vertex_data._SmallScaleWindDisplacementFactor = component->_SmallScaleWindDisplacementFactor;

			command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VegetationVertexPushConstantData), &vertex_data);

			VegetationFragmentPushConstantData fragment_data;

			fragment_data._GlobalMaterialIndex = component->_GlobalMaterialIndex;

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VegetationVertexPushConstantData), sizeof(VegetationFragmentPushConstantData), &fragment_data);

			//Bind the vertex/inder buffer.
			command_buffer->BindVertexBuffer(this, 0, mesh._VertexBuffers[0], &OFFSET);
			command_buffer->BindIndexBuffer(this, mesh._IndexBuffers[0], OFFSET);

			//Bind the transformations buffer.
			command_buffer->BindVertexBuffer(this, 1, component->_TransformationsBuffer, &OFFSET);

			command_buffer->DrawIndexed(this, mesh._IndexCounts[0], component->_NumberOfTransformations);
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}