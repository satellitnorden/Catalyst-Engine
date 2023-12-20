#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/DebugSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

//Singleton definition.
DEFINE_SINGLETON(DebugSystem);

/*
*	Initializes the debug system.
*/
void DebugSystem::Initialize() NOEXCEPT
{
	//Define constants.
	const ImVec4 PALETTE_MAIN_COLOR_HIGH{ 0.80f, 0.80f, 0.80f, 0.8f };
	const ImVec4 PALETTE_MAIN_COLOR_MEDIUM{ 0.40f, 0.40f, 0.40f, 1.0f };
	const ImVec4 PALETTE_MAIN_COLOR_LOW{ 0.20f, 0.20f, 0.20f, 1.0f };
	const ImVec4 PALETTE_BACKGROUND_COLOR{ 0.10f, 0.10f, 0.10f, 1.0f };
	const ImVec4 PALETTE_BORDER_COLOR{ PALETTE_MAIN_COLOR_MEDIUM };

	//Set up ImGui.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//Fill in ImGui's IO struct.
	ImGuiIO& io{ ImGui::GetIO() };

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.FontDefault = io.Fonts->AddFontFromFileTTF("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Fonts\\OpenSans-Regular.ttf", 18.0f);

	//Set up the style.
	ImGuiStyle *style = &ImGui::GetStyle();

	{
		auto &colors{ ImGui::GetStyle().Colors };

		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<DebugSystem *const RESTRICT>(arguments)->Update();
		},
		this,
		UpdatePhase::USER_INTERFACE,
		UpdatePhase::LOGIC,
		false,
		false
	);
}

/*
*	Terminates the debug system.
*/
void DebugSystem::Terminate() NOEXCEPT
{
	//Shut down ImGui.
	ImGui::DestroyContext();
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
	_DebugCommands.Emplace();
	DebugCommand &new_debug_command{ _DebugCommands.Back() };

	new_debug_command._Name = name;
	new_debug_command._Function = function;
	new_debug_command._UserData = user_data;
}

/*
*	Updates the debug system.
*/
void DebugSystem::Update() NOEXCEPT
{
	//Update if the debug window should be displayed.
	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::Insert) == ButtonState::PRESSED)
	{
		_DisplayDebugWindow = !_DisplayDebugWindow;
	}

	//No need to update if the debug window isn't displayed.
	if (!_DisplayDebugWindow)
	{
		return;
	}

	//Update IO.
	UpdateIO();
}

/*
*	Updates IO.
*/
void DebugSystem::UpdateIO() NOEXCEPT
{
	/*
	*	Keyboard button input character mapping class definition.
	*/
	class KeyboardButtonInputCharacterMapping final
	{

	public:

		//The keyboard button.
		KeyboardButton _KeyboardButton;

		//The upper case input character.
		char _UpperCaseInputCharacter;

		//The lower case input character.
		char _LowerCaseInputCharacter;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE constexpr KeyboardButtonInputCharacterMapping
		(
			const KeyboardButton initial_keyboard_button,
			const char initial_upper_case_input_character,
			const char initial_lower_case_input_character
		) NOEXCEPT
			:
			_KeyboardButton(initial_keyboard_button),
			_UpperCaseInputCharacter(initial_upper_case_input_character),
			_LowerCaseInputCharacter(initial_lower_case_input_character)
		{

		}
	};

	//Define constants.
	constexpr StaticArray<KeyboardButtonInputCharacterMapping, 54> KEYBOARD_BUTTON_INPUT_CHARACTER_MAPPINGS
	{
		KeyboardButtonInputCharacterMapping(KeyboardButton::A, 'A', 'a'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::B, 'B', 'b'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::C, 'C', 'c'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::D, 'D', 'd'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::E, 'E', 'e'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::F, 'F', 'f'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::G, 'G', 'g'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::H, 'H', 'h'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::I, 'I', 'i'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::J, 'J', 'J'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::K, 'K', 'k'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::L, 'L', 'l'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::M, 'M', 'm'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::N, 'N', 'n'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::O, 'O', 'o'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::P, 'P', 'p'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::Q, 'Q', 'q'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::R, 'R', 'r'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::S, 'S', 's'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::T, 'T', 't'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::U, 'U', 'u'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::V, 'V', 'v'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::W, 'W', 'w'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::X, 'X', 'x'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::Y, 'Y', 'y'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::Z, 'Z', 'z'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadZero, '0', '0'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadOne, '1', '1'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadTwo, '2', '2'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadThree, '3', '3'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadFour, '4', '4'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadFive, '5', '5'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadSix, '6', '6'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadSeven, '7', '7'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadEight, '8', '8'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NumpadNine, '9', '9'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::MULTIPLY, '*', '*'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::ADD, '+', '+'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::SEPARATOR, '.', '.'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::SUBTRACT, '-', '-'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::DECIMAL, ',', ','),
		KeyboardButtonInputCharacterMapping(KeyboardButton::DIVIDE, '/', '/'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::ZERO, '0', '0'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::ONE, '1', '1'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::TWO, '2', '2'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::THREE, '3', '3'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::FOUR, '4', '4'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::FIVE, '5', '5'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::SIX, '6', '6'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::SEVEN, '7', '7'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::EIGHT, '8', '8'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::NINE, '9', '9'),
		KeyboardButtonInputCharacterMapping(KeyboardButton::COMMA, ',', ','),
		KeyboardButtonInputCharacterMapping(KeyboardButton::PERIOD, '.', '.')
	};

	//Fill in ImGui's IO struct.
	ImGuiIO& io{ ImGui::GetIO() };

	io.DisplaySize.x = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width);
	io.DisplaySize.y = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);
	io.DeltaTime = CatalystBaseMath::Maximum<float32>(CatalystEngineSystem::Instance->GetDeltaTime(), FLOAT32_EPSILON);
	io.IniFilename = nullptr;
	io.MousePos = ImVec2(InputSystem::Instance->GetMouseState()->_CurrentX * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), (1.0f - InputSystem::Instance->GetMouseState()->_CurrentY) * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height));
	io.MouseDown[0] = InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED_HELD;
	io.MouseDown[1] = InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED_HELD;
	io.MouseDown[2] = InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED_HELD;
	io.MouseWheel = static_cast<float32>(InputSystem::Instance->GetMouseState()->_ScrollWheelStep);
	io.KeyCtrl = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightControl) == ButtonState::PRESSED;
	io.KeyShift = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED;
	io.KeyAlt = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftAlt) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightAlt) == ButtonState::PRESSED;
	io.KeySuper = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftWindows) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightWindows) == ButtonState::PRESSED;

	//Update the KeyMap.
	io.KeyMap[ImGuiKey_Tab] = UNDERLYING(KeyboardButton::Tab);
	io.KeyMap[ImGuiKey_LeftArrow] = UNDERLYING(KeyboardButton::LeftArrow);
	io.KeyMap[ImGuiKey_RightArrow] = UNDERLYING(KeyboardButton::RightArrow);
	io.KeyMap[ImGuiKey_UpArrow] = UNDERLYING(KeyboardButton::UpArrow);
	io.KeyMap[ImGuiKey_DownArrow] = UNDERLYING(KeyboardButton::DownArrow);
	io.KeyMap[ImGuiKey_PageUp] = UNDERLYING(KeyboardButton::PageUp);
	io.KeyMap[ImGuiKey_PageDown] = UNDERLYING(KeyboardButton::PageDown);
	io.KeyMap[ImGuiKey_Home] = UNDERLYING(KeyboardButton::Home);
	io.KeyMap[ImGuiKey_End] = UNDERLYING(KeyboardButton::Home);
	io.KeyMap[ImGuiKey_Insert] = UNDERLYING(KeyboardButton::Insert);
	io.KeyMap[ImGuiKey_Delete] = UNDERLYING(KeyboardButton::Delete);
	io.KeyMap[ImGuiKey_Backspace] = UNDERLYING(KeyboardButton::Backspace);
	io.KeyMap[ImGuiKey_Space] = UNDERLYING(KeyboardButton::Spacebar);
	io.KeyMap[ImGuiKey_Enter] = UNDERLYING(KeyboardButton::Enter);
	io.KeyMap[ImGuiKey_Escape] = UNDERLYING(KeyboardButton::Escape);
	io.KeyMap[ImGuiKey_KeyPadEnter] = UNDERLYING(KeyboardButton::Enter);
	io.KeyMap[ImGuiKey_A] = UNDERLYING(KeyboardButton::A);
	io.KeyMap[ImGuiKey_C] = UNDERLYING(KeyboardButton::C);
	io.KeyMap[ImGuiKey_V] = UNDERLYING(KeyboardButton::V);
	io.KeyMap[ImGuiKey_X] = UNDERLYING(KeyboardButton::X);
	io.KeyMap[ImGuiKey_Y] = UNDERLYING(KeyboardButton::Y);
	io.KeyMap[ImGuiKey_Z] = UNDERLYING(KeyboardButton::Z);

	//Update the KeyDown array.
	for (uint8 i{ 0 }; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
	{
		io.KeysDown[i] = InputSystem::Instance->GetKeyboardState()->GetButtonState(static_cast<KeyboardButton>(i)) == ButtonState::PRESSED;
	}

	//Add the input characters.
	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED
		|| InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED_HELD
		|| InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED
		|| InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED_HELD)
	{
		for (const KeyboardButtonInputCharacterMapping& mapping : KEYBOARD_BUTTON_INPUT_CHARACTER_MAPPINGS)
		{
			if (InputSystem::Instance->GetKeyboardState()->GetButtonState(mapping._KeyboardButton) == ButtonState::PRESSED)
			{
				io.AddInputCharacter(mapping._UpperCaseInputCharacter);
			}
		}
	}

	else
	{
		for (const KeyboardButtonInputCharacterMapping& mapping : KEYBOARD_BUTTON_INPUT_CHARACTER_MAPPINGS)
		{
			if (InputSystem::Instance->GetKeyboardState()->GetButtonState(mapping._KeyboardButton) == ButtonState::PRESSED)
			{
				io.AddInputCharacter(mapping._LowerCaseInputCharacter);
			}
		}
	}
}
#endif