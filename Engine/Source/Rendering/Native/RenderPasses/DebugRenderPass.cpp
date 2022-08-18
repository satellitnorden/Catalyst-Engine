#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugRenderPass);

/*
*	Default constructor.
*/
DebugRenderPass::DebugRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::DEBUG, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DebugRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DebugRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		DebugRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void DebugRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DebugGraphicsPipeline);

	//Initialize all pipelines.
	_DebugGraphicsPipeline.Initialize(static_cast<uint32>(Mode::NONE), SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());
}

/*
*	Executes this render pass.
*/
void DebugRenderPass::Execute() NOEXCEPT
{	
	//Should the mode be switched?
	if (_CurrentMode != _RequestedMode)
	{
		switch (_RequestedMode)
		{
			case Mode::NONE:
			{
				_DebugGraphicsPipeline.Terminate();

				break;
			}

			case Mode::ALBEDO:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());

				break;
			}

			case Mode::THICKNESS:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());

				break;
			}

			case Mode::NORMAL:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget());

				break;
			}

			case Mode::DEPTH:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget());

				break;
			}

			case Mode::ROUGHNESS:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures3RenderTarget());

				break;
			}

			case Mode::METALLIC:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures3RenderTarget());

				break;
			}

			case Mode::AMBIENT_OCCLUSION:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures3RenderTarget());

				break;
			}

			case Mode::EMISSIVE:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), SceneFeaturesRenderPass::Instance->GetSceneFeatures3RenderTarget());

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		_CurrentMode = _RequestedMode;
	}

	//Set execution depending on the current mode.
	if (_CurrentMode != Mode::NONE)
	{
		_DebugGraphicsPipeline.Execute();
		SetEnabled(true);
	}

	else
	{
		SetEnabled(false);
	}
}

/*
*	Terminates this render pass.
*/
void DebugRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	if (_CurrentMode != Mode::NONE)
	{
		_DebugGraphicsPipeline.Terminate();
	}
}
#endif