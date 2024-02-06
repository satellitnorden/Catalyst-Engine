#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/DebugSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

//Singleton definition.
DEFINE_SINGLETON(DebugSystem);

/*
*	Initializes the debug system.
*/
void DebugSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<DebugSystem *const RESTRICT>(arguments)->Update();
		},
		this,
		UpdatePhase::USER_INTERFACE,
		UpdatePhase::PHYSICS,
		false,
		false
	);
}

/*
*	Registers a debug command.
*/
void DebugSystem::RegisterDebugCommand
(
	const char *const RESTRICT name,
	DebugCommandFunction function,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Remember some stuff.
	DebugCategory *RESTRICT parent_category{ &_RootDebugCategory };
	std::string current_name{ name };

	for (;;)
	{
		//Figure out the category.
		const size_t first_slash_position{ current_name.find_first_of("\\") };

		if (first_slash_position != std::string::npos)
		{
			const std::string category_name{ current_name.substr(0, first_slash_position) };

			bool category_exists{ false };

			for (DebugCategory &sub_category : parent_category->_SubCategories)
			{
				if (StringUtilities::IsEqual(sub_category._Name.Data(), category_name.data()))
				{
					parent_category = &sub_category;
					current_name = current_name.substr(first_slash_position + 1);

					category_exists = true;

					break;
				}
			}

			if (!category_exists)
			{
				parent_category->_SubCategories.Emplace();
				DebugCategory &new_sub_category{ parent_category->_SubCategories.Back() };

				new_sub_category._Name = category_name.data();

				parent_category = &new_sub_category;
				current_name = current_name.substr(first_slash_position + 1);
			}
		}

		else
		{
			parent_category->_DebugCommands.Emplace();
			DebugCommand &new_debug_command{ parent_category->_DebugCommands.Back() };

			new_debug_command._Name = current_name.data();
			new_debug_command._Function = function;
			new_debug_command._UserData = user_data;

			break;
		}
	}
}

/*
*	Updates the debug system.
*/
void DebugSystem::Update() NOEXCEPT
{
	if (RenderingSystem::Instance->GetCurrentRenderingPath() == RenderingPath::CUSTOM)
	{
		return;
	}

	//Update if the debug window should be displayed.
	if (InputSystem::Instance->GetKeyboardState(InputLayer::DEBUG)->GetButtonState(KeyboardButton::Insert) == ButtonState::PRESSED)
	{
		_DisplayDebugWindow = !_DisplayDebugWindow;

		if (_DisplayDebugWindow)
		{
			InputSystem::Instance->LockInputLayer(InputLayer::GAME);
			InputSystem::Instance->ShowCursor(InputLayer::DEBUG);
		}

		else
		{
			InputSystem::Instance->UnlockInputLayer(InputLayer::GAME);
		}
	}

	//No need to update if the debug window isn't displayed.
	if (!_DisplayDebugWindow)
	{
		return;
	}

	//Draw the debug window.
	DrawDebugWindow();
}

/*
*	Draws the debug window.
*/
void DebugSystem::DrawDebugWindow() NOEXCEPT
{
	//Retrieve the window resolution.
	const Vector2<float32> window_resolution{ static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height) };

	//Add the window.
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(window_resolution._X * 0.8f, 0.0f));
	ImGui::SetWindowSize(ImVec2(window_resolution._X * 0.2f, window_resolution._Y));

	//Draw the root category.
	DrawDebugCategory(_RootDebugCategory, true);

	ImGui::End();
}

/*
*	Draws a debug category.
*/
void DebugSystem::DrawDebugCategory(DebugCategory &debug_category, const bool is_root) NOEXCEPT
{
	if (is_root || ImGui::CollapsingHeader(debug_category._Name.Data()))
	{
		for (DebugCategory &sub_category : debug_category._SubCategories)
		{
			DrawDebugCategory(sub_category, false);
		}

		for (DebugCommand &debug_command : debug_category._DebugCommands)
		{
			if (ImGui::Button(debug_command._Name.Data()))
			{
				debug_command._Function(&debug_command, debug_command._UserData);
			}
		}
	}
}
#endif