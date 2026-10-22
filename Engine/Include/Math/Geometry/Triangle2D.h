#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class Triangle2D final
{

public:

	//The vertices.
	StaticArray<Vector2<float32>, 3> _Vertices;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Triangle2D() NOEXCEPT
	{

	}

	/*
	*	Returns if the given point is inside.
	*/
	FORCE_INLINE static NO_DISCARD bool IsInside(const Triangle2D &triangle, const Vector2<float32> point) NOEXCEPT
	{
		//Calculate the edge vectors.
		const Vector2<float32> edge_vector_1{ triangle._Vertices[0] - triangle._Vertices[1] };
		const Vector2<float32> edge_vector_2{ triangle._Vertices[1] - triangle._Vertices[2] };
		const Vector2<float32> edge_vector_3{ triangle._Vertices[2] - triangle._Vertices[0] };

		//Calculate the determinants.
		const float32 determinant_1{ Determinant(edge_vector_1, point - triangle._Vertices[0]) };
		const float32 determinant_2{ Determinant(edge_vector_2, point - triangle._Vertices[1]) };
		const float32 determinant_3{ Determinant(edge_vector_3, point - triangle._Vertices[2]) };

		//The point is inside only if the three determinants are all positive.
		return determinant_1 >= 0.0f && determinant_2 >= 0.0f && determinant_3 >= 0.0f;
	}

	/*
	*	Calculates the barycentric coordinates for a point in a triangle.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> CalculateBarycentricCoordinates(const Triangle2D &triangle, const Vector2<float32> point) NOEXCEPT
	{
		//Calculate the edge vectors.
		const Vector2<float32> edge_1{ triangle._Vertices[0] - triangle._Vertices[1] };
		const Vector2<float32> edge_2{ triangle._Vertices[0] - triangle._Vertices[2] };
		const Vector2<float32> edge_3{ point - triangle._Vertices[0] };

		//Calculate the denominator and it's reciprocal.
		const float32 denominator{ Determinant(edge_1, edge_2) };
		const float32 denominator_reciprocal{ 1.0f / denominator };

		//Calculate and return the output.
		Vector3<float32> output;

		output._Y = Determinant(edge_2, edge_3) * denominator_reciprocal;
		output._Z = Determinant(edge_3, edge_1) * denominator_reciprocal;
		output._X = 1.0f - output._Y - output._Z;

		return output;
	}

private:

	/*
	*	Calculates the determinant of the given vectors.
	*/
	FORCE_INLINE static NO_DISCARD float32 Determinant(const Vector2<float32> A, const Vector2<float32> B) NOEXCEPT
	{
		return A._X * B._Y - A._Y * B._X;
	}

};