#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Print.h>
#include <Core/General/StaticString.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>

//Third party.
#include <ThirdParty/b_stacktrace/b_stacktrace.h>

//Windows.
#include <Lmcons.h>
#include <Xinput.h>

//Intrinsics.
#include <intrin.h>

//Static variable definitions.
HINSTANCE CatalystPlatformWindows::_Instance;
DynamicArray<char> CatalystPlatformWindows::_InputCharacters;

//Catalyst Windows data.
namespace CatalystWindowsData
{
	//Denotes whether or not the window is in focus.
	bool _IsWindowInFocus{ true };

	//Denotes the cursor's requested visibility.
	bool _CursorRequestedVisibility{ true };

	//Denotes the cursor's current visibility.
	bool _CursorCurrentVisibility{ true };

	//The cached cursor position.
	POINT _CachedCursorPosition{ 0, 0 };

}

/*
*	Sets the visibility of the cursor.
*/
FORCE_INLINE void SetCursorVisibility(const bool visibility) NOEXCEPT
{
	//Set the requested visibility.
	CatalystWindowsData::_CursorRequestedVisibility = visibility;
}

/*
*	Returns the CPU information.
*/
NO_DISCARD CatalystPlatform::CPUInformation CatalystPlatform::GetCPUInformation() NOEXCEPT
{
	CPUInformation cpu_information;

	//Retrieve if this CPU supports AVX2 instructions.
	{
		int32 cpu_info[4];
		__cpuid(cpu_info, 0);

		if (cpu_info[0] >= 7)
		{
			__cpuidex(cpu_info, 7, 0);

			cpu_information._SupportsAVX2 = (cpu_info[1] & (1 << 5)) != 0;
		}
	}

	return cpu_information;
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Allocate an appropriate amount of memory for keyboard events.
	CatalystPlatformWindows::_InputCharacters.Reserve(32);
}

/*
*	Updates the platform before everything else.
*/
void CatalystPlatform::PlatformPreUpdate() NOEXCEPT
{

}

/*
*	Updates the platform after everything else.
*/
void CatalystPlatform::PlatformPostUpdate() NOEXCEPT
{
	
}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{

}

/*
*	Returns the directory path of the current executable.
*/
NO_DISCARD bool CatalystPlatform::GetExecutableDirectorypath(StaticString<MAXIMUM_FILE_PATH_LENGTH> *const RESTRICT result) NOEXCEPT
{
	char file_path[MAX_PATH];

	if (GetModuleFileNameA(CatalystPlatformWindows::_Instance, file_path, sizeof(file_path)) == 0)
	{
		return false;
	}

	const uint64 file_path_length{ StringUtilities::StringLength(file_path) };

	for (int64 i{ static_cast<int64>(file_path_length) - 1 }; i >= 0; --i)
	{
		if (file_path[i] == '\\')
		{
			file_path[i] = '\0';

			break;
		}
	}

	SNPRINTF(result->Data(), result->Size(), "%s", file_path);

	return true;
}

/*
*	Destroys a platform window.
*/
void CatalystPlatform::DestroyPlayformWindow(void *const RESTRICT window) NOEXCEPT
{
	DestroyWindow(static_cast<HWND>(window));
}

/*
*	Returns the default resolution.
*/
void CatalystPlatform::GetDefaultResolution(uint32 *const RESTRICT width, uint32 *const RESTRICT height) NOEXCEPT
{
	//Retrieve the desktop window.
	const HWND desktop_window{ GetDesktopWindow() };

	if (!desktop_window)
	{
		ASSERT(false, "Panik!");

		*width = 1'920;
		*height = 1'080;

		return;
	}

	//Get the window rectangle.
	RECT window_rectangle;
	GetWindowRect(desktop_window, &window_rectangle);

	//Set the width/height.
	*width = window_rectangle.right;
	*height = window_rectangle.bottom;
}

/*
*	Returns the default refresh rate.
*/
float32 CatalystPlatform::GetDefaultRefreshRate() NOEXCEPT
{
	DEVMODE dev_mode;

	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev_mode);

	return static_cast<float32>(dev_mode.dmDisplayFrequency);
}

/*
*	Returns the number of input characters for this frame.
*/
NO_DISCARD uint64 CatalystPlatform::NumberOfInputCharacters() NOEXCEPT
{
	return CatalystPlatformWindows::_InputCharacters.Size();
}

/*
*	Returns the input character at the given index for this frame.
*/
NO_DISCARD char CatalystPlatform::InputCharacterAt(const uint64 index) NOEXCEPT
{
	return CatalystPlatformWindows::_InputCharacters[index];
}

/*
*	Hides the cursor.
*/
void CatalystPlatform::HideCursor() NOEXCEPT
{
	SetCursorVisibility(false);
}

/*
*	Shows the cursor.
*/
void CatalystPlatform::ShowCursor() NOEXCEPT
{
	SetCursorVisibility(true);
}

/*
*	Returns whether or not the cursor is shown.
*/
bool CatalystPlatform::IsCursorShown() NOEXCEPT
{
	return CatalystWindowsData::_CursorCurrentVisibility;
}

/*
*	Retrieves the user name.
*/
bool CatalystPlatform::RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT
{
	char username[UNLEN + 1];
	DWORD username_length{ UNLEN + 1 };

	if (GetUserNameA(username, &username_length))
	{
		output->SetLength(username_length);

		for (uint64 i{ 0 }; i < username_length; ++i)
		{
			(*output)[i] = username[i];
		}

		return true;
	}

	else
	{
		return false;
	}
}

void CatalystPlatform::ShowBannerAd() NOEXCEPT
{
	//Do nothing. (:
}

/*
*	Shows an interstitial ad.
*/
void CatalystPlatform::ShowInterstitialAd() NOEXCEPT
{
	//Do nothing. (:
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT format, ...) NOEXCEPT
{
	/*
	*	To guard against invalid format specifiers, we put the message in a staging buffer first and filter it.
	*/
	char format_buffer[4096];

	{
		const uint64 format_length{ strlen(format) };

		for (uint64 i{ 0 }; i < format_length; ++i)
		{
			//Guard against '%_'.
			if (format[i] == '%' && (i + 1) < format_length && format[i + 1] == '_')
			{
				format_buffer[i] = '_';
			}

			//Guard against '%%'.
			if (format[i] == '%' && (i + 1) < format_length && format[i + 1] == '%')
			{
				format_buffer[i] = '_';
			}

			else
			{
				format_buffer[i] = format[i];
			}
		}

		format_buffer[format_length] = '\0';
	}

	va_list variadic_arguments{ nullptr };
	va_start(variadic_arguments, format);

	char buffer[4096];

	const int32 characters_needed{ vsnprintf(buffer, ARRAY_LENGTH(buffer), format, variadic_arguments) };

	if (characters_needed > ARRAY_LENGTH(buffer))
	{
		int x = 0;
	}

	va_end(variadic_arguments);

	const uint64 length{ strlen(buffer) };

	buffer[length] = '\n';
	buffer[length + 1] = '\0';

	OutputDebugString(buffer);
}
#endif

/*
*	The crash handler.
*/
LONG WINAPI CatalystPlatformWindows::CrashHandler(EXCEPTION_POINTERS *exception_info) NOEXCEPT
{
	LOG_FATAL("Crash detected!");

	LOG_INFORMATION("Callstack:");

	char *callstack{ b_stacktrace_get_string() };
	LOG_INFORMATION(callstack);
	free(callstack);

	LogSystem::Instance->Flush();

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif