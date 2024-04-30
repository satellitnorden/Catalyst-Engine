#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/ImGuiSystem.h>

//Systems.
#if defined(CATALYST_EDITOR)
#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>
#if defined(CATALYST_EDITOR)
#include <ThirdParty/ImGuizmo/ImGuizmo.h>
#endif

/*
*	Initializes the ImGui system.
*/
void ImGuiSystem::Initialize() NOEXCEPT
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
	ImGuiIO &io{ ImGui::GetIO() };

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.FontDefault = io.Fonts->AddFontFromFileTTF("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Source\\Base\\Fonts\\OpenSans-Regular.ttf", 18.0f);

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

	//Set up the editor window data.
	_EditorWindowData[UNDERLYING(EditorWindow::VIEWPORT)]._Minimum = Vector2<float32>(0.0f, 0.0f);
	_EditorWindowData[UNDERLYING(EditorWindow::VIEWPORT)]._Maximum = Vector2<float32>(0.8f, 0.9f);
	_EditorWindowData[UNDERLYING(EditorWindow::VIEWPORT)]._WindowCallback = nullptr;

	_EditorWindowData[UNDERLYING(EditorWindow::TOP_BAR)]._Minimum = Vector2<float32>(0.0f, 0.9'125f);
	_EditorWindowData[UNDERLYING(EditorWindow::TOP_BAR)]._Maximum = Vector2<float32>(0.8f, 1.0f);
	_EditorWindowData[UNDERLYING(EditorWindow::TOP_BAR)]._WindowCallback = nullptr;

	_EditorWindowData[UNDERLYING(EditorWindow::TOP_RIGHT)]._Minimum = Vector2<float32>(0.8f, 0.5f);
	_EditorWindowData[UNDERLYING(EditorWindow::TOP_RIGHT)]._Maximum = Vector2<float32>(1.0f, 1.0f);
	_EditorWindowData[UNDERLYING(EditorWindow::TOP_RIGHT)]._WindowCallback = nullptr;

	_EditorWindowData[UNDERLYING(EditorWindow::BOTTOM_RIGHT)]._Minimum = Vector2<float32>(0.8f, 0.0f);
	_EditorWindowData[UNDERLYING(EditorWindow::BOTTOM_RIGHT)]._Maximum = Vector2<float32>(1.0f, 0.5f);
	_EditorWindowData[UNDERLYING(EditorWindow::BOTTOM_RIGHT)]._WindowCallback = nullptr;

	_EditorWindowData[UNDERLYING(EditorWindow::FLOATING)]._Minimum = Vector2<float32>(0.25f, 0.25f);
	_EditorWindowData[UNDERLYING(EditorWindow::FLOATING)]._Maximum = Vector2<float32>(0.75f, 0.75f);
	_EditorWindowData[UNDERLYING(EditorWindow::FLOATING)]._WindowCallback = nullptr;

	//Set up the game window data.
	_GameWindowData[UNDERLYING(GameWindow::RIGHT)]._Minimum = Vector2<float32>(0.8f, 0.0f);
	_GameWindowData[UNDERLYING(GameWindow::RIGHT)]._Maximum = Vector2<float32>(1.0f, 1.0f);
	_GameWindowData[UNDERLYING(GameWindow::RIGHT)]._WindowCallback = nullptr;

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<ImGuiSystem *const RESTRICT>(arguments)->UserInterfaceUpdate();
		},
		this,
		UpdatePhase::USER_INTERFACE,
		UpdatePhase::PHYSICS,
		true,
		false
	);

#if defined(CATALYST_EDITOR)
	//Set up ImGuizmo.
	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	{
		ImGuizmo::Style &style{ ImGuizmo::GetStyle() };

		style.Colors[ImGuizmo::COLOR::DIRECTION_X] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::DIRECTION_Y] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::DIRECTION_Z] = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::PLANE_X] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::PLANE_Y] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::PLANE_Z] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		style.Colors[ImGuizmo::COLOR::SELECTION] = ImVec4(1.0f, 0.25f, 0.0f, 1.0f);

		style.TranslationLineThickness = 2.0f;
		style.TranslationLineArrowSize = 8.0f;
		style.HatchedAxisLineThickness = 0.0f;
	}
#endif
}

/*
*	Post-terminates the ImGui system.
*/
void ImGuiSystem::PostTerminate() NOEXCEPT
{
	//Shut down ImGui.
	ImGui::DestroyContext();
}

/*
*	Callback for when input is available.
*/
void ImGuiSystem::OnInputAvailable() NOEXCEPT
{
	//Cache the input state.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE) };
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE) };

	//Fill in ImGui's IO struct.
	ImGuiIO &io{ ImGui::GetIO() };

	io.DisplaySize.x = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width);
	io.DisplaySize.y = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);
	io.DeltaTime = BaseMath::Maximum<float32>(CatalystEngineSystem::Instance->GetDeltaTime(), FLOAT32_EPSILON);
	io.IniFilename = nullptr;
	io.MousePos = ImVec2(mouse_state->_CurrentX * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), (1.0f - mouse_state->_CurrentY) * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height));
	io.MouseDown[0] = mouse_state->_Left == ButtonState::PRESSED || mouse_state->_Left == ButtonState::PRESSED_HELD;
	io.MouseDown[1] = mouse_state->_Right == ButtonState::PRESSED || mouse_state->_Right == ButtonState::PRESSED_HELD;
	io.MouseDown[2] = mouse_state->_ScrollWheel == ButtonState::PRESSED || mouse_state->_ScrollWheel == ButtonState::PRESSED_HELD;
	io.MouseWheel = static_cast<float32>(mouse_state->_ScrollWheelStep);
	io.KeyCtrl = keyboard_state->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::RightControl) == ButtonState::PRESSED;
	io.KeyShift = keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED;
	io.KeyAlt = keyboard_state->GetButtonState(KeyboardButton::LeftAlt) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::RightAlt) == ButtonState::PRESSED;
	io.KeySuper = keyboard_state->GetButtonState(KeyboardButton::LeftWindows) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::RightWindows) == ButtonState::PRESSED;

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
	io.KeyMap[ImGuiKey_KeypadEnter] = UNDERLYING(KeyboardButton::Enter);
	io.KeyMap[ImGuiKey_A] = UNDERLYING(KeyboardButton::A);
	io.KeyMap[ImGuiKey_C] = UNDERLYING(KeyboardButton::C);
	io.KeyMap[ImGuiKey_V] = UNDERLYING(KeyboardButton::V);
	io.KeyMap[ImGuiKey_X] = UNDERLYING(KeyboardButton::X);
	io.KeyMap[ImGuiKey_Y] = UNDERLYING(KeyboardButton::Y);
	io.KeyMap[ImGuiKey_Z] = UNDERLYING(KeyboardButton::Z);

	//Update the KeyDown array.
	for (uint8 i{ 0 }; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
	{
		io.KeysDown[i] = keyboard_state->GetButtonState(static_cast<KeyboardButton>(i)) == ButtonState::PRESSED;
	}

	//Add input characters.
	for (uint64 i{ 0 }; i < CatalystPlatform::NumberOfInputCharacters(); ++i)
	{
		io.AddInputCharacter(CatalystPlatform::InputCharacterAt(i));
	}

	//Begin the new ImGui frame.
	ImGui::NewFrame();

#if defined(CATALYST_EDITOR)
	//Update ImGuizmo.
	ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::BeginFrame();
#endif
}

/*
*	Registers an editor window.
*/
void ImGuiSystem::RegisterEditorWindow(const EditorWindow window, const WindowCallback callback) NOEXCEPT
{
	_EditorWindowData[UNDERLYING(window)]._WindowCallback = callback;
}

/*
*	Utility function for beginning an ImGui window.
*/
bool ImGuiSystem::BeginWindow(const BeginWindowParameters &parameters) NOEXCEPT
{
	//Calculate the window flags.
	ImGuiWindowFlags window_flags = 0;

	//Remove title bar if requested.
	if (!parameters._ShowTitleBar)
	{
		window_flags |= ImGuiWindowFlags_NoTitleBar;
	}

	//Always no resize.
	if (!parameters._EnableResize)
	{
		window_flags |= ImGuiWindowFlags_NoResize;
	}

	//Always no move.
	if (!parameters._EnableMove)
	{
		window_flags |= ImGuiWindowFlags_NoMove;
	}

	//Always no collapse.
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//Enable menu bar, if requested.
	if (parameters._EnableMenuBar)
	{
		window_flags |= ImGuiWindowFlags_MenuBar;
	}

	//Begin the window!
	bool should_be_open{ true };

	ImGui::Begin
	(
		parameters._Name,
		parameters._Closable ? &should_be_open : nullptr,
		window_flags
	);

	//Retrieve the window resolution.
	const Vector2<float32> window_resolution{ static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height) };

	//Calculate the window position.
	Vector2<float32> window_position{ window_resolution._X * parameters._Minimum._X, window_resolution._Y * (1.0f - parameters._Maximum._Y) };

	//Calculate the window size.
	Vector2<float32> window_size{ window_resolution._X * (parameters._Maximum._X - parameters._Minimum._X), window_resolution._Y * (parameters._Maximum._Y - parameters._Minimum._Y) };

	//Set the window position and size.
	ImGui::SetWindowPos(ImVec2(window_position._X, window_position._Y), ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2(window_size._X, window_size._Y), ImGuiCond_Once);
	//ImGui::SetWindowPos(ImVec2(64, 128));
	//ImGui::SetWindowSize(ImVec2(1920, 64));

	if (parameters._Closable)
	{
		return should_be_open;
	}

	else
	{
		return true;
	}
}

/*
*	Updates during the USER_INTERFACE update phase.
*/
void ImGuiSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Update window data.
#if defined(CATALYST_EDITOR)
	if (!CatalystEditorSystem::Instance->IsInGame())
	{
		for (uint32 window_data_index{ 0 }; window_data_index < static_cast<uint32>(_EditorWindowData.Size()); ++window_data_index)
		{
			WindowData &window_data{ _EditorWindowData[window_data_index] };

			if (window_data._WindowCallback)
			{
				if (!window_data._WindowCallback(window_data._Minimum, window_data._Maximum))
				{
					window_data._WindowCallback = nullptr;
				}
			}
		}
	}

	else
#endif
	{
		for (uint32 window_data_index{ 0 }; window_data_index < static_cast<uint32>(_GameWindowData.Size()); ++window_data_index)
		{
			WindowData &window_data{ _GameWindowData[window_data_index] };

			if (window_data._WindowCallback)
			{
				if (!window_data._WindowCallback(window_data._Minimum, window_data._Maximum))
				{
					window_data._WindowCallback = nullptr;
				}
			}
		}
	}
}
#endif