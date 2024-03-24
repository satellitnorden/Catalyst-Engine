#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/General/EulerAngles.h>
#include <Math/General/Vector.h>

//Scripting.
#include <Scripting/ScriptContext.h>

namespace Script
{
	namespace Math
	{
		NO_DISCARD float32 Distance(ScriptContext &script_context, const Vector3<float32> &A, const Vector3<float32> &B) NOEXCEPT;
		NO_DISCARD float32 Lerp(ScriptContext &script_context, const float32 A, const float32 B, const float32 alpha) NOEXCEPT;
		NO_DISCARD float32 Sine(ScriptContext &script_context, const float32 phase) NOEXCEPT;
	}

	namespace Script
	{

		/*
		*	Sends an event to linked entities.
		*/
		void Event(ScriptContext &script_context, const HashString event) NOEXCEPT;
	
	}

	namespace StaticModel
	{
		void SetMaterial(ScriptContext &script_context, const uint8 mesh_index, const HashString material) NOEXCEPT;
	}

	namespace WorldTransform
	{
		NO_DISCARD Vector3<float32> GetPosition(ScriptContext &script_context) NOEXCEPT;
		void SetRotation(ScriptContext &script_context, const EulerAngles& rotation) NOEXCEPT;
		void SetScale(ScriptContext &script_context, const float32 scale) NOEXCEPT;
	}
}