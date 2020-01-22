#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

class TerrainErosion final
{

public:

	/*
	*	Simulation parameters class definition.
	*/
	class SimulationParameters final
	{

	public:

		//The number of iterations.
		uint64 _Iterations{ 1'024 };

		//The maximum lifetime of a particle.
		uint64 _MaximumParticleLifetime{ 32 };

		//The inertia.
		float _Intertia{ 0.05f };

		//The capacity.
		float _Capacity{ 10.0f };

		//The gravity.
		float _Gravity{ 4.0f };

		//The evaporation.
		float _Evaporation{ 0.1f };

		//The erosion.
		float _Erosion{ 0.25f };

		//The erosion.
		float _Deposition{ 0.25f };

		//The minimum slope.
		float _MinimumSlope{ 0.0001f };

	};

	/*
	*	Performs an erosion simulation on the given height map.
	*/
	FORCE_INLINE static void SimulateErosion(const SimulationParameters &parameters, const uint32 resolution, float* const RESTRICT height_map) NOEXCEPT
	{
		for (uint64 i{ 0 }; i < parameters._Iterations; ++i)
		{
			Particle particle;

			particle._Position._X = CatalystRandomMath::RandomFloatInRange(0.0f, static_cast<float>(resolution));
			particle._Position._Y = CatalystRandomMath::RandomFloatInRange(0.0f, static_cast<float>(resolution));
			particle._Direction._X = 0.0f;
			particle._Direction._Y = 0.0f;
			particle._Velocity = 0.0f;
			particle._Sediment = 0.0f;
			particle._Water = 1.0f;

			for (uint64 j{ 0 }; j < parameters._MaximumParticleLifetime; ++j)
			{
				Vector2<float> pos_old = particle._Position;
				GradientHeight hg = height_gradient_at(resolution, height_map, pos_old);
				Vector2<float> g = hg._Gradient;
				float h_old = hg._Height;

				particle._Direction = (particle._Direction * parameters._Intertia) - (g * (1.0f - parameters._Intertia));

				particle._Direction.Normalize();

				particle._Position += particle._Direction;

				Vector2<float> pos_new = particle._Position;

				if (pos_new._X > (resolution - 1) || pos_new._X < 0
					|| pos_new._Y >(resolution - 1) || pos_new._Y < 0)
				{
					break;
				}

				float h_new = bil_interpolate_map_double(resolution, height_map, pos_new);
				float h_diff = h_new - h_old;

				// sediment capacity
				float c = fmax(-h_diff, parameters._MinimumSlope) * particle._Velocity * particle._Water * parameters._Capacity;

				// decide whether to erode or deposit depending on particle properties
				if (h_diff > 0 || particle._Sediment > c)
				{
					float to_deposit = (h_diff > 0) ?
						fmin(particle._Sediment, h_diff) :
						(particle._Sediment - c) * parameters._Deposition;
					particle._Sediment -= to_deposit;
					deposit(resolution, height_map, pos_old, to_deposit);
				}

				else
				{
					float to_erode = fmin((c - particle._Sediment) * parameters._Erosion, -h_diff);
					particle._Sediment += to_erode;
					erode(resolution, height_map, pos_old, to_erode);
				}

				// update `vel` and `water`
				particle._Velocity = sqrt(particle._Velocity * particle._Velocity + h_diff * parameters._Gravity);
				particle._Water *= (1.0f - parameters._Evaporation);
			}
		}
	}

private:

	/*
	*	Gradient/height class definition.
	*/
	class GradientHeight final
	{

	public:

		//The gradient.
		Vector2<float> _Gradient;

		//The height.
		float _Height;

	};

	/*
	*	Particle class definition.
	*/
	class Particle final
	{

	public:

		//The position.
		Vector2<float> _Position;

		//The direction.
		Vector2<float> _Direction;

		//The velocity.
		float _Velocity;

		//The sediment.
		float _Sediment;

		//The water.
		float _Water;

	};

	/*
	*	Bilinearly interpolate float value at (x, y) in map.
	*/
	FORCE_INLINE static float bil_interpolate_map_double(const uint32 resolution, const float *const RESTRICT height_map, const Vector2<float> position)
	{
		float u, v, ul, ur, ll, lr, ipl_l, ipl_r;
		int32 x_i = (int32)position._X;
		int32 y_i = (int32)position._Y;
		u = position._X - x_i;
		v = position._Y - y_i;
		ul = height_map[y_i * resolution + x_i];
		ur = height_map[y_i * resolution + x_i + 1];
		ll = height_map[(y_i + 1) * resolution + x_i];
		lr = height_map[(y_i + 1) * resolution + x_i + 1];
		ipl_l = (1.0f - v) * ul + v * ll;
		ipl_r = (1.0f - v) * ur + v * lr;

		return (1.0f - u) * ipl_l + u * ipl_r;
	}

	/*
	*	Deposits sediment at position `pos` in heighmap `hmap`.
	*	Deposition only affect immediate neighbouring gridpoints
	*	to `pos`.
	*/
	FORCE_INLINE static void deposit(const uint32 resolution, float *const RESTRICT height_map, const Vector2<float> position, const float amount)
	{
		int32 x_i = (int)position._X;
		int32 y_i = (int)position._Y;
		float u = position._X - x_i;
		float v = position._Y - y_i;
		height_map[y_i * resolution + x_i] += amount * (1 - u) * (1 - v);
		height_map[y_i * resolution + x_i + 1] += amount * u * (1 - v);
		height_map[(y_i + 1) * resolution + x_i] += amount * (1 - u) * v;
		height_map[(y_i + 1) * resolution + x_i + 1] += amount * u * v;
	}

	/*
	*	Erodes heighmap `hmap` at position `pos` by amount `amount`.
	*	Erosion is distributed over an area defined through p_radius.
	*/
	FORCE_INLINE static void erode(const uint32 resolution, float *const RESTRICT height_map, const Vector2<float> position, const float amount)
	{
		int32 x0 = (int32)position._X - 2;
		int32 y0 = (int32)position._Y - 2;
		int32 x_start = CatalystBaseMath::Maximum<int32>(0, x0);
		int32 y_start = CatalystBaseMath::Maximum<int32>(0, y0);
		int32 x_end = CatalystBaseMath::Minimum<int32>(resolution, x0 + 2 * 3 + 1);
		int32 y_end = CatalystBaseMath::Minimum<int32>(resolution, y0 + 2 * 3 + 1);

		// construct erosion/deposition kernel.
		float kernel[2 * 3 + 1][2 * 3 + 1];
		float kernel_sum = 0;
		for (int y = y_start; y < y_end; y++)
		{
			for (int x = x_start; x < x_end; x++)
			{
				float d_x = x - position._X;
				float d_y = y - position._Y;
				float distance = sqrt(d_x * d_x + d_y * d_y);
				float w = fmax(0, 2 - distance);
				kernel_sum += w;
				kernel[y - y0][x - x0] = w;
			}
		}

		// normalize weights and apply changes on heighmap.
		for (int32 y = y_start; y < y_end; y++)
		{
			for (int32 x = x_start; x < x_end; x++)
			{
				kernel[y - y0][x - x0] /= kernel_sum;
				height_map[y * resolution + x] -= amount * kernel[y - y0][x - x0];
			}
		}
	}

	/*
	*	Returns gradient at (int x, int y) on heightmap `hmap`.
	*/
	FORCE_INLINE static Vector2<float> gradient_at(const uint32 resolution, float *const RESTRICT height_map, int32 x, int32 y)
	{
		int32 idx = y * resolution + x;
		int right = idx + ((x > resolution - 2) ? 0 : 1);
		int below = idx + ((y > resolution - 2) ? 0 : resolution);
		Vector2<float> g;
		g._X = height_map[right] - height_map[idx];
		g._Y = height_map[below] - height_map[idx];

		return g;
	}

	/*
	*	Returns interpolated gradient and height at (double x, double y) on
	*	heightmap `hmap`.
	*/
	FORCE_INLINE static GradientHeight height_gradient_at(const uint32 resolution, float *const RESTRICT height_map, const Vector2<float> position)
	{
		GradientHeight ret;
		Vector2<float> ul, ur, ll, lr, ipl_l, ipl_r;
		int32 x_i = (int32)position._X;
		int32 y_i = (int32)position._Y;
		float u = position._X - x_i;
		float v = position._Y - y_i;
		ul = gradient_at(resolution, height_map, x_i, y_i);
		ur = gradient_at(resolution, height_map, x_i + 1, y_i);
		ll = gradient_at(resolution, height_map, x_i, y_i + 1);
		lr = gradient_at(resolution, height_map, x_i + 1, y_i + 1);
		ipl_l = (ul * (1.0f - v)) + (ll * v);
		ipl_r = (ur * (1.0f - v)) + (lr * v);
		ret._Gradient = (ipl_l * (1.0f - u)) + (ipl_r * u);
		ret._Height = bil_interpolate_map_double(resolution, height_map, position);
		return ret;
	}

};