//Header file.
#include <Rendering/Engine/RenderPasses/TerrainDepthRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainDepthRenderPass);


/*
*	Default constructor.
*/
TerrainDepthRenderPass::TerrainDepthRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		TerrainDepthRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the terrain depth render pass.
*/
void TerrainDepthRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::TerrainDepth);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TerrainDepthFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneIntermediate);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(8);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32SignedInt,
										sizeof(Vector2));
	AddVertexInputAttributeDescription(	2,
										1,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	3,
										1,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										sizeof(Vector2));
	AddVertexInputAttributeDescription(	4,
										1,
										VertexInputAttributeDescription::Format::X32SignedInt,
										sizeof(Vector2) + sizeof(float));
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32SignedInt,
										sizeof(Vector2) + sizeof(float) + sizeof(int32));
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										sizeof(Vector2) + sizeof(float) + sizeof(int32) + sizeof(int32));
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										sizeof(Vector2) + sizeof(float) + sizeof(int32) + sizeof(int32) + sizeof(float));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(TerrainPatchInstanceRenderInformation), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(BIT(0) | BIT(1));
	SetStencilReferenceMask(BIT(0) | BIT(1));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		TerrainDepthRenderPass::Instance->RenderInternal();
	});

	//Initialize the instance buffers.
	InitializeInstanceBuffers();

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Initializes the instance buffers.
*/
void TerrainDepthRenderPass::InitializeInstanceBuffers() NOEXCEPT
{
	//Get the number of framebuffers.
	const uint8 numberOfFrameBuffers{ RenderingSystem::Instance->GetNumberOfFrameBuffers() };

	//Create all instance buffers.
	_InstanceBuffers.Reserve(numberOfFrameBuffers);

	for (uint8 i{ 0 }; i < numberOfFrameBuffers; ++i)
	{
		_InstanceBuffers.EmplaceFast(RenderingSystem::Instance->CreateUniformBuffer(sizeof(TerrainPatchInstanceRenderInformation) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, BufferUsage::VertexBuffer));
	}
}

/*
*	Renders the depth of the terrain.
*/
void TerrainDepthRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all terrain render informations and draw them if they're low detail.
	const DynamicArray<TerrainPatchRenderInformation> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	//If there's none to draw, draw none!
	if (informations->Empty())
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Bind vertex/inder buffer.
	constexpr uint64 offset{ 0 };

	commandBuffer->BindVertexBuffer(this, 0, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, &offset);
	commandBuffer->BindIndexBuffer(this, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, TerrainSystem::Instance->GetTerrainProperties()->_IndexOffset);

	//Wait for terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	//Create a list of non-culled patches and add them to the instances list.
	StaticArray<TerrainPatchInstanceRenderInformation, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES> _InstanceInformations;
	uint32 numberOfInstanceInformations{ 0 };

	MemoryUtilities::SetMemory(_InstanceInformations.Data(), 0, sizeof(TerrainPatchInstanceRenderInformation) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES);

	for (const TerrainPatchRenderInformation &information : *informations)
	{
		if (!TEST_BIT(information._Visibility, VisibilityFlag::Viewer))
		{
			continue;
		}

		_InstanceInformations[numberOfInstanceInformations++] = information._InstanceInformation;
	}

	//Upload the instance informations to the current instance buffer.
	RenderingSystem::Instance->UploadDataToUniformBuffer(_InstanceBuffers[RenderingSystem::Instance->GetCurrentFrameBufferIndex()], _InstanceInformations.Data());

	//Bind the instance buffer!
	commandBuffer->BindVertexBuffer(this, 1, _InstanceBuffers[RenderingSystem::Instance->GetCurrentFrameBufferIndex()], &offset);

	//Draw all patches!
	commandBuffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, numberOfInstanceInformations);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}