#if defined(CATALYST_WINDOWS)
//Header file.
#include <Platform/Windows/CatalystWindows.h>

//Core.
#include <Core/EngineCore.h>

//Systems.
#include <Systems/EngineSystem.h>

//Windows.
#include <tchar.h>

//Static variable definitions.
HINSTANCE CatalystPlatform::instance;
int32 CatalystPlatform::showCommand;
HWND CatalystPlatform::window;

/*
*	Window procedure handling.
*/
LRESULT CALLBACK WindowProcedure(	_In_ HWND   window,
									_In_ UINT   message,
									_In_ WPARAM wParam,
									_In_ LPARAM lParam) NOEXCEPT
{
	switch (message)
	{
		case WM_DESTROY:
		{
			EngineSystem::Instance->Terminate();

			return DefWindowProc(window, message, wParam, lParam);
		}

		default:
		{
			return DefWindowProc(window, message, wParam, lParam);
		}
	}
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Create the window info.
	WNDCLASSEX windowInfo;

	windowInfo.cbSize = sizeof(WNDCLASSEX);
	windowInfo.style = CS_HREDRAW | CS_VREDRAW;
	windowInfo.lpfnWndProc = WindowProcedure;
	windowInfo.cbClsExtra = 0;
	windowInfo.cbWndExtra = 0;
	windowInfo.hInstance = CatalystPlatform::instance;
	windowInfo.hIcon = LoadIcon(CatalystPlatform::instance, IDI_APPLICATION);
	windowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowInfo.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowInfo.lpszMenuName = NULL;
	windowInfo.lpszClassName = _T("Catalyst Engine");
	windowInfo.hIconSm = LoadIcon(windowInfo.hInstance, IDI_APPLICATION);

#if !defined(CATALYST_FINAL)
	if (!RegisterClassEx(&windowInfo))
	{
		BREAKPOINT;
	}
#endif

	//Create the window.
	window = CreateWindow(	_T("Catalyst Engine"),
							_T(EngineSystem::Instance->GetProjectConfiguration().generalConfiguration.projectName.CString()),
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							1'920,
							1'080,
							nullptr,
							nullptr,
							instance,
							nullptr);

#if !defined(CATALYST_FINAL)
	if (!window)
	{
		const DWORD result{ GetLastError() };

		BREAKPOINT;
	}
#endif

	//Show the window.
	ShowWindow(window, showCommand);

	//Update the window.
	UpdateWindow(window);
}

/*
*	Post-nitializes the platform.
*/
void CatalystPlatform::PostInitialize() NOEXCEPT
{

}

/*
*	Pre-updates the platform.
*/
void CatalystPlatform::PreUpdate() NOEXCEPT
{

}

/*
*	Post-updates the platform.
*/
void CatalystPlatform::PostUpdate() NOEXCEPT
{
	//Process messages.
	MSG message;

	while (PeekMessage(&message, window, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

/*
*	Releases the platform.
*/
void CatalystPlatform::Release() NOEXCEPT
{

}

#if !defined(CATALYST_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT message) NOEXCEPT
{
	OutputDebugString(message);
}
#endif
#endif