#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics.
#include <Physics/PhysicsCore.h>

//Catalyst physics math constants.
namespace CatalystPhysicsMathConstants
{
	constexpr float GRAVITATIONAL_SPEED{ 9.80665f };
	constexpr Vector3<float> GRAVITATIONAL_DIRECTION{ 0.0f, -1.0f, 0.0f };
}

namespace CatalystPhysicsMath
{

	/*
	*	Given mass and acceleration, calculates the force using Newton's second law of motion.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateForce(const float mass, const Vector3<float> &acceleration) NOEXCEPT
	{
		return mass * acceleration;
	}

	/*
	*	Given force and acceleration, calculates the mass using Newton's second law of motion.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float CalculateMass(const Vector3<float> &force, const Vector3<float> &acceleration) NOEXCEPT
	{
		return Vector3<float>::Length(force / acceleration);
	}

	/*
	*	Given force and mass, calculates the acceleration using Newton's second law of motion.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateAcceleration(const Vector3<float> &force, const float mass) NOEXCEPT
	{
		return force / mass;
	}

	/*
	*	Given the mass of an object, calculate the gravitational force.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateGravitationalForce(const float mass) NOEXCEPT
	{
		return mass * (CatalystPhysicsMathConstants::GRAVITATIONAL_DIRECTION * CatalystPhysicsMathConstants::GRAVITATIONAL_SPEED);
	}

	/*
	*	Returns if two physics shapes are colliding.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool IsColliding(const PhysicsShape firstShape, const Matrix4 &firstTransform, const PhysicsShape secondShape, const Matrix4 &secondTransform) NOEXCEPT
	{
		return false;

		/*
		return	box1._Minimum._X <= box2._Maximum._X
				&& box1._Maximum._X >= box2._Minimum._X
				&& box1._Minimum._Y <= box2._Maximum._Y
				&& box1._Maximum._Y >= box2._Minimum._Y
				&& box1._Minimum._Z <= box2._Maximum._Z
				&& box1._Maximum._Z >= box2._Minimum._Z;
		*/
	}

}