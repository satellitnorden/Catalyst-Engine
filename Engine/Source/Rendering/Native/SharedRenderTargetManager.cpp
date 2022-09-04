//Header file.
#include <Rendering/Native/SharedRenderTargetManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes the shared render target manager.
*/
void SharedRenderTargetManager::Initialize(const RenderingPath initial_rendering_path) NOEXCEPT
{
	//Reset the shared render targets.
	for (RenderTargetHandle &shared_render_target : _SharedRenderTargets)
	{
		shared_render_target = EMPTY_HANDLE;
	}

	//Set up the shared render target informations.
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::PATH_TRACING)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::PATH_TRACING)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::PATH_TRACING)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._TextureFormat = TextureFormat::RG_FLOAT16;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::PATH_TRACING)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;

	//Create the required shared render targets.
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (_SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(initial_rendering_path)])
		{
			RenderingSystem::Instance->CreateRenderTarget
			(
				_SharedRenderTargetInformations[i]._Resolution,
				_SharedRenderTargetInformations[i]._TextureFormat,
				&_SharedRenderTargets[i]
			);
		}
	}
}

/*
*	Called when switching rendering path.
*/
void SharedRenderTargetManager::OnSwitchRenderingPath(const RenderingPath new_rendering_path) NOEXCEPT
{
	//Are any shared render targets no longer needed?
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (_SharedRenderTargets[i]
			&& !_SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(new_rendering_path)])
		{
			RenderingSystem::Instance->DestroyRenderTarget(&_SharedRenderTargets[i]);
		}
	}

	//Are any shared render targets needed?
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (!_SharedRenderTargets[i]
			&& _SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(new_rendering_path)])
		{
			RenderingSystem::Instance->CreateRenderTarget
			(
				_SharedRenderTargetInformations[i]._Resolution,
				_SharedRenderTargetInformations[i]._TextureFormat,
				&_SharedRenderTargets[i]
			);
		}
	}
}