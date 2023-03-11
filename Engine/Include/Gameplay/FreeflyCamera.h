#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class FreeflyCamera final
{

public:

	/*
	*	Initializes this freefly camera.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates this freefly camera.
	*/
	void Update(const float32 delta_time) NOEXCEPT;

	/*
	*	Sets the rotation factor.
	*/
	FORCE_INLINE void SetRotationFactor(const float32 value) NOEXCEPT
	{
		_RotationFactor = value;
	}

private:

	/*
	*	Freefly camera input state definition.
	*/
	class FreeflyCameraInputState final
	{

	public:

		//The speed increment.
		int8 _SpeedIncrement{ 0 };

		//The rotation.
		Vector2<float32> _Rotation{ 0.0f, 0.0f };

		//The movement.
		Vector3<float32> _Movement{ 0.0f, 0.0f, 0.0f };

	};

	//The rotation factor.
	float32 _RotationFactor{ 1.0f };

	//The current input state.
	FreeflyCameraInputState _CurrentInputState;

	//The rotation.
	EulerAngles _Rotation;

	//The current speed.
	float32 _CurrentSpeed{ 16.0f };

	/*
	*	Updates the current input state.
	*/
	void UpdateCurrentInputState(const float32 delta_time) NOEXCEPT;

};