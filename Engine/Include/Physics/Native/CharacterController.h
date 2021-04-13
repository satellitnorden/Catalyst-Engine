#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

//World.
#include <World/Core/WorldPosition.h>

class CharacterController final
{

public:

	//Define constants.
	constexpr static uint64 ABSTRACTION_DATA_SIZE{ 16 };

	/*
	*	Constructor taking the abstraction data as argument.
	*/
	FORCE_INLINE CharacterController(const Any<ABSTRACTION_DATA_SIZE> &initial_abstraction_data) NOEXCEPT
		:
		_AbstractionData(initial_abstraction_data)
	{

	}

	/*
	*	Returns the world position.
	*/
	NO_DISCARD WorldPosition GetWorldPosition() NOEXCEPT;

	/*
	*	Sets the world position.
	*/
	void SetWorldPosition(const WorldPosition &value) NOEXCEPT;

	/*
	*	Returns if the character is on ground.
	*/
	NO_DISCARD bool IsOnGround() NOEXCEPT;

	/*
	*	Moves the character controller.
	*/
	void Move(const Vector3<float32> &displacement) NOEXCEPT;

private:

	//The abstraction data.
	Any<ABSTRACTION_DATA_SIZE> _AbstractionData;

};