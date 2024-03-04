#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/EulerAngles.h>

//Scripting.
#include <Scripting/ScriptContext.h>

namespace Script
{
	namespace Math
	{
		NO_DISCARD float32 Lerp(ScriptContext &script_context, const float32 A, const float32 B, const float32 alpha) NOEXCEPT;
		NO_DISCARD float32 Sine(ScriptContext &script_context, const float32 phase) NOEXCEPT;
	}

	namespace WorldTransform
	{
		void SetRotation(ScriptContext &script_context, const EulerAngles& rotation) NOEXCEPT;
	}
}