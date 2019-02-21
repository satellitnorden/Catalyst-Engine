//Header file.
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>

//Core.
#include <Core/General/Padding.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ParticleSystemRenderPass);

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	Vector3<float> _MinimumPosition;
	Padding<4> _Padding1;
	Vector3<float> _MaximumPosition;
	Padding<4> _Padding2;
	Vector3<float> _MinimumVelocity;
	Padding<4> _Padding3;
	Vector3<float> _MaximumVelocity;
	Padding<4> _Padding4;
	Vector3<float> _WorldPosition;
	Padding<4> _Padding5;
	Vector2<float> _MinimumScale;
	Vector2<float> _MaximumScale;
	float _FadeTime;
	float _Lifetime;
	float _SpawnFrequency;
	int32 _Properties;
	float _RandomSeed;
	float _TotalTime;

};

static_assert(offsetof(PushConstantData, _MinimumPosition) == 0, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _MaximumPosition) == 16, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _MinimumVelocity) == 32, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _MaximumVelocity) == 48, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _WorldPosition) == 64, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _MinimumScale) == 80, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _MaximumScale) == 88, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _FadeTime) == 96, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _Lifetime) == 100, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _SpawnFrequency) == 104, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _Properties) == 108, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _RandomSeed) == 112, "Offset is wrong. ):");
static_assert(offsetof(PushConstantData, _TotalTime) == 116, "Offset is wrong. ):");

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
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::ParticleSystem);

	//Set the shaders.
	SetVertexShader(Shader::ParticleSystemVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::ParticleSystemGeometry);
	SetFragmentShader(Shader::ParticleSystemFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(4);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);
	AddRenderTarget(RenderTarget::SceneProperties);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Geometry, 0, sizeof(PushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Front);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT | RenderingConstants::SCENE_BUFFER_PARTICLE_SYSTEMS_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT | RenderingConstants::SCENE_BUFFER_PARTICLE_SYSTEMS_STENCIL_BIT);
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
	const ParticleSystemRenderComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for particle systems culling to finish.
	CullingSystem::Instance->WaitForParticleSystemsCulling();

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		if (!TEST_BIT(component->_Visibility, VisibilityFlag::Perceiver))
		{
			continue;
		}

		//Push constants.
		PushConstantData data;

		data._MinimumPosition = component->_Properties._MinimumPosition;
		data._MaximumPosition = component->_Properties._MaximumPosition;
		data._MinimumVelocity = component->_Properties._MinimumVelocity;
		data._MaximumVelocity = component->_Properties._MinimumVelocity;
		data._WorldPosition = component->_Properties._WorldPosition;
		data._MinimumScale = component->_Properties._MinimumScale;
		data._MaximumScale = component->_Properties._MaximumScale;
		data._FadeTime = component->_Properties._FadeTime;
		data._Lifetime = component->_Properties._Lifetime;
		data._SpawnFrequency = component->_Properties._SpawnFrequency;
		data._Properties = static_cast<int32>(component->_Properties._Properties);
		data._RandomSeed = component->_ParticleSystemRandomSeed;
		data._TotalTime = CatalystEngineSystem::Instance->GetTotalTime() - component->_ParticleSystemStartingTime;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(PushConstantData), &data);

		commandBuffer->BindRenderDataTable(this, 1, component->_RenderDataTable);
		commandBuffer->Draw(this, 1, component->_InstanceCount);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}