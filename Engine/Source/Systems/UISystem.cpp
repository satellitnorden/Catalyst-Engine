#if defined(USE_NEW_UI_SYSTEM)
//Header file.
#include <Systems/UISystem.h>

/*
*	Updates the user interface system.
*/
void UISystem::Update(const UpdatePhase phase) NOEXCEPT
{
	//Process the add scene requests.
	{
		Optional<UI::Scene *RESTRICT> add_scene_request{ _AddSceneRequests.Pop() };

		while (add_scene_request.Valid())
		{
			_Scenes.Emplace(add_scene_request.Get());

			add_scene_request = _AddSceneRequests.Pop();
		}
	}

	//Process the remove scene requests.
	{
		Optional<UI::Scene *RESTRICT> remove_scene_request{ _RemoveSceneRequests.Pop() };

		while (remove_scene_request.Valid())
		{
			UI::Scene *const RESTRICT scene{ remove_scene_request.Get() };

			_Scenes.Erase<true>(scene);
			delete scene;

			remove_scene_request = _RemoveSceneRequests.Pop();
		}
	}

	//Prepare all scenes for building.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->PrepareBuild();
	}

	//Prepare the widget allocator.
	_WidgetAllocator.Prepare();

	//Set up the build context.
	UI::BuildContext context;

	context._WidgetAllocator = &_WidgetAllocator;

	//Build all scenes.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->Build(context);
	}

	//Clean the widget allocator.
	_WidgetAllocator.Clean();
}
#endif