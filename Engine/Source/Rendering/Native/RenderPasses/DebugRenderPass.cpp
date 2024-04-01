#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

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

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	_DebugGraphicsPipeline.Initialize(static_cast<uint32>(Mode::NONE), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Initialize();
	}
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

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

				break;
			}

			case Mode::THICKNESS:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

				break;
			}

			case Mode::NORMAL:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));

				break;
			}

			case Mode::DEPTH:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));

				break;
			}

			case Mode::ROUGHNESS:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

				break;
			}

			case Mode::METALLIC:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

				break;
			}

			case Mode::AMBIENT_OCCLUSION:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

				break;
			}

			case Mode::EMISSIVE:
			{
				if (_CurrentMode != Mode::NONE)
				{
					_DebugGraphicsPipeline.Terminate();
				}

				_DebugGraphicsPipeline.Initialize(static_cast<uint32>(_RequestedMode), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

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
		_DebugGraphicsPipeline.SetIncludeInRender(true);
		_DebugGraphicsPipeline.Execute();
	}

	else
	{
		_DebugGraphicsPipeline.SetIncludeInRender(false);
	}

	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Execute();
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

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Terminate();
	}
}
#endif