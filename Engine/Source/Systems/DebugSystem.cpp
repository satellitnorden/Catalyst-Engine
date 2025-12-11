#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/DebugSystem.h>

//Systems.
#include <Systems/ImGuiSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ImGui/imgui.h>

//STL.
#include <string>

/*
*	Updates the debug system.
*/
void DebugSystem::Update(const UpdatePhase phase) NOEXCEPT
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

			ImGuiSystem::Instance->RegisterGameWindow
			(
				ImGuiSystem::GameWindow::RIGHT,
				[](const Vector2<float32> minimum, const Vector2<float32> maximum)
				{
					return DebugSystem::Instance->DrawDebugWindow(minimum, maximum);
				}
			);
		}

		else
		{
			InputSystem::Instance->UnlockInputLayer(InputLayer::GAME);
		}
	}
}

/*
*	Registers a button debug command.
*/
void DebugSystem::RegisterButtonDebugCommand
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

			new_debug_command._FullName = name;
			new_debug_command._Name = current_name.data();
			new_debug_command._Type = DebugCommand::Type::BUTTON;
			new_debug_command._Function = function;
			new_debug_command._UserData = user_data;

			break;
		}
	}
}

/*
*	Registers a checkbox debug command.
*/
void DebugSystem::RegisterCheckboxDebugCommand
(
	const char *const RESTRICT name,
	DebugCommandFunction function,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Remember some stuff.
	DebugCategory* RESTRICT parent_category{ &_RootDebugCategory };
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

			new_debug_command._FullName = name;
			new_debug_command._Name = current_name.data();
			new_debug_command._Type = DebugCommand::Type::CHECKBOX;
			new_debug_command._Function = function;
			new_debug_command._UserData = user_data;

			break;
		}
	}
}

/*
*	Registers a slider debug command.
*/
void DebugSystem::RegisterSliderDebugCommand
(
	const char *const RESTRICT name,
	DebugCommandFunction function,
	void *const RESTRICT user_data,
	const float32 minimum,
	const float32 maximum,
	const float32 initial_value
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

			new_debug_command._FullName = name;
			new_debug_command._Name = current_name.data();
			new_debug_command._Type = DebugCommand::Type::SLIDER;
			new_debug_command._Function = function;
			new_debug_command._UserData = user_data;
			new_debug_command._State._SliderState._Minimum = minimum;
			new_debug_command._State._SliderState._Maximum = maximum;
			new_debug_command._State._SliderState._Value = initial_value;

			break;
		}
	}
}

/*
*	Draws the debug window.
*/
NO_DISCARD bool DebugSystem::DrawDebugWindow(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::BeginWindowParameters parameters;

	parameters._Name = "Debug";
	parameters._Minimum = minimum;
	parameters._Maximum = maximum;

	ImGuiSystem::Instance->BeginWindow(parameters);

	//Draw the root category.
	DrawDebugCategory(_RootDebugCategory, true, 0);

	ImGui::End();

	return _DisplayDebugWindow;
}

/*
*	Draws a debug category.
*/
void DebugSystem::DrawDebugCategory(DebugCategory &debug_category, const bool is_root, const uint8 depth) NOEXCEPT
{
	//Add some spacing.
	for (uint8 i{ 0 }; i < depth; ++i)
	{
		ImGui::Text(" ");
		ImGui::SameLine();
	}

	if (is_root || ImGui::CollapsingHeader(debug_category._Name.Data()))
	{
		for (DebugCategory &sub_category : debug_category._SubCategories)
		{
			DrawDebugCategory(sub_category, false, depth + (is_root ? 0 : 1));
		}

		for (DebugCommand &debug_command : debug_category._DebugCommands)
		{
			//Add some spacing.
			for (uint8 i{ 0 }; i < depth; ++i)
			{
				ImGui::Text(" ");
				ImGui::SameLine();
			}

			//Push the ID.
			ImGui::PushID(debug_command._FullName.Data());

			switch (debug_command._Type)
			{
				case DebugCommand::Type::BUTTON:
				{
					if (ImGui::Button(debug_command._Name.Data()))
					{
						debug_command._Function(&debug_command, debug_command._UserData);
					}

					break;
				}

				case DebugCommand::Type::CHECKBOX:
				{
					if (ImGui::Checkbox(debug_command._Name.Data(), &debug_command._State._CheckboxState._IsChecked))
					{
						debug_command._Function(&debug_command, debug_command._UserData);
					}

					break;
				}

				case DebugCommand::Type::SLIDER:
				{
					if (ImGui::SliderFloat
					(
						debug_command._Name.Data(),
						&debug_command._State._SliderState._Value,
						debug_command._State._SliderState._Minimum,
						debug_command._State._SliderState._Maximum
					))
					{
						debug_command._Function(&debug_command, debug_command._UserData);
					}

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
			
			//Pop the ID.
			ImGui::PopID();
		}
	}
}
#endif