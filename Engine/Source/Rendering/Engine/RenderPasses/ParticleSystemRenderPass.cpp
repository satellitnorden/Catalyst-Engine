//Header file.
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ParticleSystemRenderPass);

/*
*	Default constructor.
*/
ParticleSystemRenderPass::ParticleSystemRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		ParticleSystemRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the particle system render pass.
*/
void ParticleSystemRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::ParticleSystem);

	//Set the sub stage index.
	SetSubStageIndex(7);

	//Set the shaders.
	SetVertexShader(Shader::ParticleSystemVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::ParticleSystemGeometry);
	SetFragmentShader(Shader::ParticleSystemFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::Scene);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(3);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(renderDataTableLayout);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Geometry, 0, sizeof(float) * 2);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetCullMode(CullMode::Front);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetTopology(Topology::PointList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		ParticleSystemRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Creates the render data table layout.
*/
void ParticleSystemRenderPass::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &renderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void ParticleSystemRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(renderDataTableLayout, &renderDataTable);

	RenderingSystem::Instance->UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::RenderTarget, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene)), renderDataTable);
	RenderingSystem::Instance->UpdateRenderDataTable(RenderDataTableUpdateInformation(1, RenderDataTableUpdateInformation::Type::RenderTarget, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth)), renderDataTable);
}

/*
*	Renders the particle systems.
*/
void ParticleSystemRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all particle system components and draw them all.
	const uint64 numberOfParticleSystemComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to render - render none.
	if (numberOfParticleSystemComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const ParticleSystemRenderComponent *RESTRICT component{ ComponentManager::GetParticleSystemRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, renderDataTable);

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		struct ParticleSystemData
		{
			float particleSystemRandomSeed;
			float particleSystemTotalTime;
		} particleSystemData;

		particleSystemData.particleSystemRandomSeed = component->particleSystemRandomSeed;
		particleSystemData.particleSystemTotalTime = EngineSystem::Instance->GetTotalGameTime() - component->particleSystemStartingTime;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(float) * 2, &particleSystemData);
		commandBuffer->BindRenderDataTable(this, 2, component->renderDataTable);
		commandBuffer->Draw(this, 1, component->instanceCount);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}