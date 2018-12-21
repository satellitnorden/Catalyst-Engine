#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Physics.
#include <Physics/Master/PhysicsCore.h>

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
	*	Given an incoming direction and a normel, calculate the reflected direction.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateReflectedDirection(const Vector3<float> &incoming, const Vector3<float> &normal) NOEXCEPT
	{
		return incoming - 2.0f * Vector3<float>::DotProduct(incoming, normal) * normal;
	}

}