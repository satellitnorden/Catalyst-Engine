#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>
#include <Math/General/Vector.h>

/*
*	Samples a poisson disk where a minimum distance is specified and then N samples are generated enforcing this minimum distance.
*/
class PoissonDiskSampler final
{

public:

	/*
	*	Runs this poisson disk sampler.
	*/
	FORCE_INLINE void Run(const float32 minimum_distance, DynamicArray<Vector2<float32>> *const RESTRICT points) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 NUMBER_OF_CANDIDATES{ 12 };

		//Initialize the cells.
		InitializeCells(minimum_distance);

		//Calculate the squared minimum distance.
		const float32 squared_minimum_distance{ minimum_distance * minimum_distance };

		//Set up the active points.
		DynamicArray<Vector2<float32>> active_points;

		//Add the first point.
		{
			const Vector2<float32> point{ _RandomNumberGenerator.RandomFloat<float32>(), _RandomNumberGenerator.RandomFloat<float32>() };

			points->Emplace(point);
			active_points.Emplace(point);
			AddPoint(point);
		}

		//Keep filling up points until none can fit anymore.
		while (!active_points.Empty())
		{
			//Pick a random active point.
			const uint64 random_active_point_index{ _RandomNumberGenerator.RandomIntegerInRange<uint64>(0, active_points.LastIndex()) };
			const Vector2<float32> random_active_point{ active_points[random_active_point_index] };
			
			//Calculate the rotation jitter.
			const float32 rotation_jitter{ BaseMathConstants::DOUBLE_PI / static_cast<float32>(NUMBER_OF_CANDIDATES) * _RandomNumberGenerator.RandomFloat<float32>() };

			//Generate a number of candidates around the active point.
			bool any_valid_candidate{ false };

			for (uint64 candidate_index{ 0 }; candidate_index < NUMBER_OF_CANDIDATES; ++candidate_index)
			{
				Vector2<float32> candidate_direction{ 1.0f, 0.0f };
				candidate_direction.Rotate(static_cast<float32>(candidate_index) / static_cast<float32>(NUMBER_OF_CANDIDATES) * BaseMathConstants::DOUBLE_PI + rotation_jitter);
				const Vector2<float32> candidate_point{ random_active_point + candidate_direction * _RandomNumberGenerator.RandomFloatInRange<float32>(minimum_distance, minimum_distance * 2.0f) };

				if (IsPointValid(candidate_point, squared_minimum_distance))
				{
					points->Emplace(candidate_point);
					active_points.Emplace(candidate_point);
					AddPoint(candidate_point);

					any_valid_candidate = true;
				}
			}

			if (!any_valid_candidate)
			{
				active_points.EraseAt<false>(random_active_point_index);
			}
		}
	}

private:

	/*
	*	Cell class definition.
	*/
	class Cell final
	{

	public:

		//Denotes whether or not this cell has a point.
		bool _HasPoint;

		//The point.
		Vector2<float32> _Point;

	};

	//The cell size.
	float32 _CellSize;

	//The number of cells.
	uint32 _NumberOfCells;

	//The cells.
	DynamicArray<DynamicArray<Cell>> _Cells;

	//The random number generator.
	RandomNumberGenerator _RandomNumberGenerator;

	/*
	*	Initializes the cells.
	*/
	FORCE_INLINE void InitializeCells(const float32 minimum_distance) NOEXCEPT
	{
		//Calculate the cell size.
		_CellSize = minimum_distance / std::sqrt(2.0f);

		//Calculate the number of cells.
		_NumberOfCells = static_cast<uint32>(1.0f / _CellSize);

		//Set up the cells.
		_Cells.Upsize<true>(_NumberOfCells);

		for (uint32 X{ 0 }; X < _NumberOfCells; ++X)
		{
			_Cells[X].Upsize<false>(_NumberOfCells);
			Memory::Set(_Cells[X].Data(), 0, sizeof(Cell) * _NumberOfCells);
		}
	}

	/*
	*	Calculates the coordinate for a given point.
	*/
	FORCE_INLINE NO_DISCARD Vector2<uint32> CalculateCoordinate(const Vector2<float32> point) NOEXCEPT
	{
		return Vector2<uint32>
		{
			BaseMath::Minimum<uint32>(static_cast<uint32>(point._X * static_cast<float32>(_NumberOfCells)), _NumberOfCells - 1),
			BaseMath::Minimum<uint32>(static_cast<uint32>(point._Y * static_cast<float32>(_NumberOfCells)), _NumberOfCells - 1)
		};
	}

	/*
	*	Adds a point.
	*/
	FORCE_INLINE void AddPoint(const Vector2<float32> point) NOEXCEPT
	{
		const Vector2<uint32> coordinate{ CalculateCoordinate(point) };

		ASSERT(!_Cells[coordinate._X][coordinate._Y]._HasPoint, "Cell already has a point!");

		if (_Cells[coordinate._X][coordinate._Y]._HasPoint) return;

		_Cells[coordinate._X][coordinate._Y]._HasPoint = true;
		_Cells[coordinate._X][coordinate._Y]._Point = point;
	}

	/*
	*	Returns if the given point is valid.
	*/
	FORCE_INLINE NO_DISCARD bool IsPointValid(const Vector2<float32> point, const float32 squared_minimum_distance) NOEXCEPT
	{
		if (point._X < 0.0f || point._X > 1.0f || point._Y < 0.0f || point._Y > 1.0f)
		{
			return false;
		}

		const Vector2<uint32> coordinate{ CalculateCoordinate(point) };

		for (int32 Y{ static_cast<int32>(coordinate._Y) - 1 }; Y <= (static_cast<int32>(coordinate._Y) + 1); ++Y)
		{
			if (Y < 0 || Y >= _NumberOfCells)
			{
				continue;
			}

			for (int32 X{ static_cast<int32>(coordinate._X) - 1 }; X <= (static_cast<int32>(coordinate._X) + 1); ++X)
			{
				if (X < 0 || X >= _NumberOfCells)
				{
					continue;
				}

				const Cell &cell{ _Cells[X][Y] };

				if (cell._HasPoint)
				{
					const float32 squared_distance{ Vector2<float32>::LengthSquared(point - cell._Point) };

					if (squared_distance < squared_minimum_distance)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

};