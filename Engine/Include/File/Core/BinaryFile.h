#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all binary file modes.
enum class BinaryFileMode : uint8
{
	IN,
	OUT
};

template <BinaryFileMode BINARY_FILE_MODE>
class BinaryFile final
{
	
};

//Platform implementations.
#if defined(CATALYST_PLATFORM_ANDROID)
	#include <Platform/Android/AndroidBinaryFile.inl>
#endif

#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
	#include <Platform/OculusQuest/OculusQuestBinaryFile.inl>
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsBinaryFile.inl>
#endif