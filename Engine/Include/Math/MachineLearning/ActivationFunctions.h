#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

namespace ActivationFunctions
{

	/*
	*	Linear.
	*
	*	Just returns X without doing anything to it. The most basic activation function. (:
	*	Pretty useless, actually.
	*
	*	Output range: [-INFINITY, INFINITY].
	*/
	FORCE_INLINE NO_DISCARD float32 Linear(const float32 X) NOEXCEPT
	{
		return X;
	}

	/*
	*	Sigmoid.
	*
	*	This takes any realy value as input and squishes it into the output range.
	*	Kinda follows an "S" shape.
	*	It is commonly used for models where it is necessary to predict the probability as an output.
	*	Since probability of anything exists only between the range of 0.0f and 1.0f,
	*	Sigmoid is the right choice because of it's range.
	*
	*	Output range: [0.0f, 1.0f].
	*/
	FORCE_INLINE NO_DISCARD float32 Sigmoid(const float32 X) NOEXCEPT
	{
		return 1.0f / (1.0f + exp(-X));
	}

	/*
	*	Hyperbolic tangent.
	*
	*	Similar to Sigmoid as in that it has the same "S" shape, but the output range differs.
	*
	*	Output range: [-0.0f, 1.0f].
	*/
	FORCE_INLINE NO_DISCARD float32 HyperbolicTangent(const float32 X) NOEXCEPT
	{
		return tanhf(X);
	}

	/*
	*	ReLU.
	*
	*	Stands for "Rectified Linear Unit".
	*	Similar to Sigmoid as in that it has the same "S" shape, but the output range differs.
	*
	*	Output range: [0.0f, INFINITY].
	*/
	FORCE_INLINE NO_DISCARD float32 ReLU(const float32 X) NOEXCEPT
	{
		return CatalystBaseMath::Maximum<float32>(0.0f, X);
	}

}