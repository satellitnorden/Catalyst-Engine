#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorContentBrowser.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/ImGuiSystem.h>

/*
*	Request browsing for an asset.
*/
void EditorContentBrowser::Request
(
	const char *const RESTRICT prompt,
	const HashString asset_type_identifier,
	const ContentBrowserCallbackFunction callback_function,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Set the data.
	_Prompt = prompt;
	_AssetTypeIdentifier = asset_type_identifier;
	_CallbackFunction = callback_function;
	_UserData = user_data;

	//Set up the filtered assets.
	_FilteredAssets.Clear();

	for (Asset *const RESTRICT asset : ContentSystem::Instance->GetAllAssetsOfType(_AssetTypeIdentifier).ValueIterator())
	{
		_FilteredAssets.Emplace(asset);
	}

	//Register for the floating editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::FLOATING,
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
	ImGuiSystem::BeginWindowParameters begin_window_parameters;

	begin_window_parameters._Name = _Prompt;
	begin_window_parameters._Minimum = minimum;
	begin_window_parameters._Maximum = maximum;
	begin_window_parameters._EnableResize = true;
	begin_window_parameters._EnableMove = true;

	ImGuiSystem::Instance->BeginWindow(begin_window_parameters);

	//Add buttons for all filtered assets.
	for (Asset *const RESTRICT asset : _FilteredAssets)
	{
		if (ImGui::Selectable(asset->_Header._AssetName.Data()))
		{
			_CallbackFunction(asset, _UserData);

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