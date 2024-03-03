#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorContentBrowser.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ImGuiSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Request browsing for an asset.
*/
void EditorContentBrowser::Request
(
	const char *const RESTRICT prompt,
	const HashString resource_identifier,
	const ContentBrowserCallbackFunction callback_function,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Set the data.
	_Prompt = prompt;
	_ResourceIdentifier = resource_identifier;
	_CallbackFunction = callback_function;
	_UserData = user_data;

	//Set up the filtered resources.
	_FilteredResources.Clear();

	for (Resource *const RESTRICT resource : ResourceSystem::Instance->GetAllResources())
	{
		if (resource->_Header._TypeIdentifier == _ResourceIdentifier)
		{
			_FilteredResources.Emplace(resource);
		}
	}

	//Register for the floating 1 editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::FLOATING_1,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->GetEditorContentBrowser()->UpdateFloatingWindow(minimum, maximum);
		}
	);

	//Should be open for now. (:
	_ShouldBeOpen = true;
}

/*
*	Updates the floating window.
*/
NO_DISCARD bool EditorContentBrowser::UpdateFloatingWindow(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::Instance->BeginWindow
	(
		_Prompt,
		minimum,
		maximum,
		true,
		true,
		true,
		false
	);

	//Add buttons for all filtered resources.
	for (Resource *const RESTRICT resource : _FilteredResources)
	{
		if (ImGui::Selectable(resource->_Header._ResourceName.Data()))
		{
			_CallbackFunction(resource, _UserData);

			_ShouldBeOpen = false;

			break;
		}
	}

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return _ShouldBeOpen;
}
#endif