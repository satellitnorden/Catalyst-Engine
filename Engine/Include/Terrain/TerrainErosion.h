#pragma once

#if 1
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Noise/PoissonDiskSampler.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/LogSystem.h>

class TerrainErosion final
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		/*
		*	The minimum bounds.
		*	Useful if you only want a specific area of the height map to be affected.
		*/
		Vector2<uint32> _MinimumBounds{ 0, 0 };

		/*
		*	The maximum bounds.
		*	Useful if you only want a specific area of the height map to be affected.
		*	Will be set to resolution - 1 if UINT32_MAXIMUM.
		*/
		Vector2<uint32> _MaximumBounds{ UINT32_MAXIMUM, UINT32_MAXIMUM };

		/*
		*	The minimum distance between points.
		*	The simulation uses Poisson disk sampling to generate a uniform-ish grid of points, decreasing this will lead to more points.
		*/
		float32 _MinimumDistanceBetweenPoints{ 0.01f };

		/*
		*	The maximum particle lifetime.
		*	The longer the lifetime, the longer a particle can travel.
		*/
		uint64 _MaximumParticleLifetime{ 32 };

		//The inertia.
		float32 _Inertia{ 0.5f };

		//The capacity.
		float32 _Capacity{ 10.0f };

		//The gravity.
		float32 _Gravity{ 4.0f };

		//The minimum slope.
		float32 _MinimumSlope{ 0.0001f };

		//The deposition.
		float32 _Deposition{ 0.5f };

		//The erosion speed.
		float32 _Erosion{ 0.5f };

		//The evaporation.
		float32 _Evaporation{ 0.1f };

	};

	/*
	*	Performs an erosion simulation on the given height map.
	*/
	FORCE_INLINE static void Simulate(const Parameters &input_parameters, Texture2D<float32> *const RESTRICT height_map) NOEXCEPT
	{
		//Copy the parameters.
		Parameters parameters{ input_parameters };

		//Generate the points.
		DynamicArray<Vector2<float32>> points;
		
		{
			PoissonDiskSampler poisson_disk_sampler;
			poisson_disk_sampler.Run(parameters._MinimumDistanceBetweenPoints, &points);
		}

		for (const Vector2<float32> point : points)
		{
			Particle particle;

			particle._Position._X = BaseMath::LinearlyInterpolate<float32>(static_cast<float32>(parameters._MinimumBounds._X), static_cast<float32>(parameters._MaximumBounds._X), point._X);
			particle._Position._Y = BaseMath::LinearlyInterpolate<float32>(static_cast<float32>(parameters._MinimumBounds._Y), static_cast<float32>(parameters._MaximumBounds._Y), point._Y);
			particle._Direction._X = 0.0f;
			particle._Direction._Y = 0.0f;
			particle._Velocity = 0.0f;
			particle._Water = 1.0f;
			particle._Sediment = 0.0f;

			for (uint64 particle_lifetime_index{ 0 }; particle_lifetime_index < parameters._MaximumParticleLifetime; ++particle_lifetime_index)
			{
				//Remember the original position.
				const Vector2<float32> original_position{ particle._Position };

				//Calculate the old gradient direction/height.
				Vector2<float32> old_gradient_direction;
				float32 old_gradient_height;
				CalculateGradient(height_map, particle._Position, &old_gradient_direction, &old_gradient_height);

				old_gradient_direction.Normalize();

				//Update the direction of the particle.
				particle._Direction = particle._Direction * parameters._Inertia + old_gradient_direction * (1.0f - parameters._Inertia);
				particle._Direction.Normalize();

				//Update the position of the particle.
				particle._Position += particle._Direction;

				//Break if the particle has wandered outside the bounds.
				if (particle._Position._X < static_cast<float32>(parameters._MinimumBounds._X) || particle._Position._X > static_cast<float32>(parameters._MaximumBounds._X)
					|| particle._Position._Y < static_cast<float32>(parameters._MinimumBounds._Y) || particle._Position._Y > static_cast<float32>(parameters._MaximumBounds._Y))
				{
					break;
				}

				//Break if the paticle has no direction.
				if (particle._Direction._X == 0.0f && particle._Direction._Y == 0.0f)
				{
					break;
				}

				//Calculate the new gradient direction/height.
				Vector2<float32> new_gradient_direction;
				float32 new_gradient_height;
				CalculateGradient(height_map, particle._Position, &new_gradient_direction, &new_gradient_height);

				//Calculate the delta height.
				const float32 delta_height{ new_gradient_height - old_gradient_height };

				//Calculate the sediment capacity.
				const float32 sediment_capacity{ BaseMath::Maximum<float32>(-delta_height, parameters._MinimumSlope) * particle._Velocity * particle._Water * parameters._Capacity };

				//Should this particle deposit?
				if (particle._Sediment > sediment_capacity || delta_height > 0.0f)
				{
					const float32 deposit_amount{ delta_height > 0.0f ? BaseMath::Minimum<float32>(delta_height, particle._Sediment) : BaseMath::Minimum<float32>(particle._Sediment - sediment_capacity, -delta_height) * parameters._Deposition };
					ASSERT(deposit_amount >= 0.0f, "Deposit amount is negative!");
					Deposit(height_map, original_position, deposit_amount);
					particle._Sediment -= deposit_amount;
				}

				//Otherwise, erade a fraction of the particle's current carry capacity.
				else
				{
					const float32 erode_amount{ BaseMath::Minimum<float32>((sediment_capacity - particle._Sediment) * parameters._Erosion, -delta_height) };
					ASSERT(erode_amount >= 0.0f, "Erode amount is negative!");
					Erode(height_map, original_position, erode_amount);
					particle._Sediment += erode_amount;
				}

				//Update the particle's velocity.
				particle._Velocity = std::sqrt(BaseMath::Maximum<float32>(particle._Velocity * particle._Velocity - delta_height * parameters._Gravity, 0.0f));
				ASSERT(!BaseMath::IsNaN(particle._Velocity), "NaN detected!");

				//Evaporate.
				particle._Water *= (1.0f - parameters._Evaporation);
			}
		}
	}

private:

	/*
	*	Particle class definition.
	*/
	class Particle final
	{

	public:

		//The position.
		Vector2<float32> _Position;

		//The direction.
		Vector2<float32> _Direction;

		//The velocity.
		float32 _Velocity;

		//The water.
		float32 _Water;

		//The sediment.
		float32 _Sediment;

	};

	/*
	*	Returns the gradient.
	*/
	FORCE_INLINE static void CalculateGradient(const Texture2D<float32> *const RESTRICT height_map, const Vector2<float32> position, Vector2<float32> *const RESTRICT gradient_direction, float32 *const RESTRICT gradient_height) NOEXCEPT
	{
		const Vector2<int32> lower_left_coordinate{ BaseMath::Clamp<int32>(static_cast<int32>(position._X), 0, height_map->GetWidth() - 1), BaseMath::Clamp<int32>(static_cast<int32>(position._Y), 0, height_map->GetHeight() - 1) };
		const Vector2<int32> upper_left_coordinate{ BaseMath::Clamp<int32>(static_cast<int32>(position._X), 0, height_map->GetWidth() - 1), BaseMath::Clamp<int32>(static_cast<int32>(position._Y) + 1, 0, height_map->GetHeight() - 1) };
		const Vector2<int32> lower_right_coordinate{ BaseMath::Clamp<int32>(static_cast<int32>(position._X) + 1, 0, height_map->GetWidth() - 1), BaseMath::Clamp<int32>(static_cast<int32>(position._Y), 0, height_map->GetHeight() - 1) };
		const Vector2<int32> upper_right_coordinate{ BaseMath::Clamp<int32>(static_cast<int32>(position._X) + 1, 0, height_map->GetWidth() - 1), BaseMath::Clamp<int32>(static_cast<int32>(position._Y) + 1, 0, height_map->GetHeight() - 1) };

		const float32 lower_left_height{ height_map->At(lower_left_coordinate._X, lower_left_coordinate._Y) };
		const float32 upper_left_height{ height_map->At(upper_left_coordinate._X, upper_left_coordinate._Y) };
		const float32 lower_right_height{ height_map->At(lower_right_coordinate._X, lower_right_coordinate._Y) };
		const float32 upper_right_height{ height_map->At(upper_right_coordinate._X, upper_right_coordinate._Y) };

		const float32 horizontal_alpha{ BaseMath::Fractional(position._X) };
		const float32 vertical_alpha{ BaseMath::Fractional(position._Y) };

		gradient_direction->_X = (lower_right_height - lower_left_height) * (1.0f - vertical_alpha) + (upper_right_height - upper_left_height) * vertical_alpha;
		gradient_direction->_Y = (upper_left_height - lower_left_height) * (1.0f - horizontal_alpha) + (upper_right_height - lower_right_height) * horizontal_alpha;

		gradient_direction->_X = -gradient_direction->_X;
		gradient_direction->_Y = -gradient_direction->_Y;

		(*gradient_height) =	lower_left_height * (1.0f - horizontal_alpha) * (1.0f - vertical_alpha)
								+ lower_right_height * horizontal_alpha * (1.0f - vertical_alpha)
								+ upper_left_height * (1.0f - horizontal_alpha) * vertical_alpha
								+ upper_right_height * horizontal_alpha * vertical_alpha;
	}

	/*
	*	Erodes at the given position.
	*/
	FORCE_INLINE static void Erode(Texture2D<float32> *const RESTRICT height_map, const Vector2<float32> position, const float32 amount) NOEXCEPT
	{
		const Vector2<uint32> lower_left_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X), height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y), height_map->GetHeight() - 1) };
		const Vector2<uint32> upper_left_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X), height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y) + 1, height_map->GetHeight() - 1) };
		const Vector2<uint32> lower_right_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X) + 1, height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y), height_map->GetHeight() - 1) };
		const Vector2<uint32> upper_right_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X) + 1, height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y) + 1, height_map->GetHeight() - 1) };

		const float32 horizontal_alpha{ BaseMath::Fractional(position._X) };
		const float32 vertical_alpha{ BaseMath::Fractional(position._Y) };

		height_map->At(lower_left_coordinate._X, lower_left_coordinate._Y) -= (amount * (1.0f - horizontal_alpha) * (1.0f - vertical_alpha));
		height_map->At(upper_left_coordinate._X, upper_left_coordinate._Y) -= (amount * (1.0f - horizontal_alpha) * vertical_alpha);
		height_map->At(lower_right_coordinate._X, lower_right_coordinate._Y) -= (amount * horizontal_alpha * (1.0f - vertical_alpha));
		height_map->At(upper_right_coordinate._X, upper_right_coordinate._Y) -= (amount * horizontal_alpha * vertical_alpha);
	}

	/*
	*	Deposits at the given position.
	*/
	FORCE_INLINE static void Deposit(Texture2D<float32> *const RESTRICT height_map, const Vector2<float32> position, const float32 amount) NOEXCEPT
	{
		const Vector2<uint32> lower_left_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X), height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y), height_map->GetHeight() - 1) };
		const Vector2<uint32> upper_left_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X), height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y) + 1, height_map->GetHeight() - 1) };
		const Vector2<uint32> lower_right_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X) + 1, height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y), height_map->GetHeight() - 1) };
		const Vector2<uint32> upper_right_coordinate{ BaseMath::Minimum<uint32>(static_cast<uint32>(position._X) + 1, height_map->GetWidth() - 1), BaseMath::Minimum<uint32>(static_cast<uint32>(position._Y) + 1, height_map->GetHeight() - 1) };

		const float32 horizontal_alpha{ BaseMath::Fractional(position._X) };
		const float32 vertical_alpha{ BaseMath::Fractional(position._Y) };

		height_map->At(lower_left_coordinate._X, lower_left_coordinate._Y) += (amount * (1.0f - horizontal_alpha) * (1.0f - vertical_alpha));
		height_map->At(upper_left_coordinate._X, upper_left_coordinate._Y) += (amount * (1.0f - horizontal_alpha) * vertical_alpha);
		height_map->At(lower_right_coordinate._X, lower_right_coordinate._Y) += (amount * horizontal_alpha * (1.0f - vertical_alpha));
		height_map->At(upper_right_coordinate._X, upper_right_coordinate._Y) += (amount * horizontal_alpha * vertical_alpha);
	}

};
#else
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/LogSystem.h>

class TerrainErosion final
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		/*
		*	The minimum bounds.
		*	Useful if you only want a specific area of the height map to be affected.
		*/
		Vector2<uint32> _MinimumBounds{ 0, 0 };

		/*
		*	The maximum bounds.
		*	Useful if you only want a specific area of the height map to be affected.
		*	Will be set to resolution - 1 if UINT32_MAXIMUM.
		*/
		Vector2<uint32> _MaximumBounds{ UINT32_MAXIMUM, UINT32_MAXIMUM };

		/*
		*	The number of iterations,
		*	or the number of particles that will be spawned.
		*/
		uint64 _Iterations{ 1'024 };

		/*
		*	The maximum particle lifetime.
		*	The longer the lifetime, the longer a particle can travel.
		*/
		uint64 _MaximumParticleLifetime{ 32 };

		//The inertia.
		float32 _Intertia{ 0.5f };

		//The capacity.
		float32 _Capacity{ 10.0f };

		//The gravity.
		float32 _Gravity{ 4.0f };

		//The evaporation.
		float32 _Evaporation{ 0.1f };

		//The erosion.
		float32 _Erosion{ 0.25f };

		//The erosion.
		float32 _Deposition{ 0.25f };

		//The minimum slope.
		float32 _MinimumSlope{ 0.0001f };

	};

	/*
	*	Performs an erosion simulation on the given height map.
	*/
	FORCE_INLINE static void Simulate(const Parameters &input_parameters, Texture2D<float32> *const RESTRICT height_map) NOEXCEPT
	{
		//Copy the parameters.
		Parameters parameters{ input_parameters };

		//If the maximum bounds are set to the default UINT32 value, set it to a proper value.
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			if (parameters._MaximumBounds[i] == UINT32_MAXIMUM)
			{
				parameters._MaximumBounds[i] = height_map->GetResolution() - 1;
			}
		}

		for (uint64 i{ 0 }; i < parameters._Iterations; ++i)
		{
			Particle particle;

			particle._Position._X = CatalystRandomMath::RandomFloatInRange(static_cast<float32>(parameters._MinimumBounds._X), static_cast<float32>(parameters._MaximumBounds._X));
			particle._Position._Y = CatalystRandomMath::RandomFloatInRange(static_cast<float32>(parameters._MinimumBounds._Y), static_cast<float32>(parameters._MaximumBounds._Y));
			particle._Direction._X = 0.0f;
			particle._Direction._Y = 0.0f;
			particle._Velocity = 0.0f;
			particle._Sediment = 0.0f;
			particle._Water = 1.0f;

			for (uint64 j{ 0 }; j < parameters._MaximumParticleLifetime; ++j)
			{
				//Cache the old position.
				const Vector2<float32> old_position{ particle._Position };

				//Calculate the gradient.
				GradientHeight hg = CalculateGradientHeight(parameters, height_map->GetResolution(), height_map->Data(), old_position);
				Vector2<float> g = hg._Gradient;
				float h_old = hg._Height;

				particle._Direction = (particle._Direction * parameters._Intertia) - (g * (1.0f - parameters._Intertia));

				particle._Direction.Normalize();

				particle._Position += particle._Direction;

				if (particle._Position._X < static_cast<float32>(parameters._MinimumBounds._X)
					|| particle._Position._X > static_cast<float32>(parameters._MaximumBounds._X)
					|| particle._Position._Y < static_cast<float32>(parameters._MinimumBounds._Y)
					|| particle._Position._Y > static_cast<float32>(parameters._MaximumBounds._Y))
				{
					break;
				}

				float h_new = BilinearInterpolate(parameters, height_map->GetResolution(), height_map->Data(), particle._Position);
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
					deposit(parameters, height_map->GetResolution(), height_map->Data(), old_position, to_deposit);
				}

				else
				{
					float to_erode = fmin((c - particle._Sediment) * parameters._Erosion, -h_diff);
					particle._Sediment += to_erode;
					erode(parameters, height_map->GetResolution(), height_map->Data(), old_position, to_erode);
				}

				//Update the particle's velocity.
				particle._Velocity = sqrt(particle._Velocity * particle._Velocity + h_diff * parameters._Gravity);

				//Update the particle's water.
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
		Vector2<float32> _Gradient;

		//The height.
		float32 _Height;

	};

	/*
	*	Particle class definition.
	*/
	class Particle final
	{

	public:

		//The position.
		Vector2<float32> _Position;

		//The direction.
		Vector2<float32> _Direction;

		//The velocity.
		float32 _Velocity;

		//The sediment.
		float32 _Sediment;

		//The water.
		float32 _Water;

	};

	/*
	*	Calculates the gradient height at the given position on the height map.
	*/
	FORCE_INLINE static GradientHeight CalculateGradientHeight(const Parameters &parameters, const uint32 resolution, float* const RESTRICT height_map, const Vector2<float> position)
	{
		GradientHeight output;
		Vector2<float> ul, ur, ll, lr, ipl_l, ipl_r;
		int32 x_i = (int32)position._X;
		int32 y_i = (int32)position._Y;
		float u = position._X - x_i;
		float v = position._Y - y_i;
		ul = CalculateGradient(parameters, resolution, height_map, x_i, y_i);
		ur = CalculateGradient(parameters, resolution, height_map, BaseMath::Minimum<uint32>(x_i + 1, parameters._MaximumBounds._X), y_i);
		ll = CalculateGradient(parameters, resolution, height_map, x_i, BaseMath::Minimum<uint32>(y_i + 1, parameters._MaximumBounds._Y));
		lr = CalculateGradient(parameters, resolution, height_map, BaseMath::Minimum<uint32>(x_i + 1, parameters._MaximumBounds._X), BaseMath::Minimum<uint32>(y_i + 1, parameters._MaximumBounds._Y));
		ipl_l = (ul * (1.0f - v)) + (ll * v);
		ipl_r = (ur * (1.0f - v)) + (lr * v);
		output._Gradient = (ipl_l * (1.0f - u)) + (ipl_r * u);
		output._Height = BilinearInterpolate(parameters, resolution, height_map, position);

		//Return the output.
		return output;
	}

	/*
	*	Calculate the gradient at the given coordinates on the height map.
	*/
	FORCE_INLINE static Vector2<float> CalculateGradient(const Parameters &parameters, const uint32 resolution, float* const RESTRICT height_map, int32 x, int32 y)
	{
		int32 idx = x + y * resolution;
		int32 right = idx + ((x > static_cast<int32>(resolution) - 2) ? 0 : 1);
		int32 above = idx + ((y > static_cast<int32>(resolution) - 2) ? 0 : resolution);
		Vector2<float> g;
		g._X = height_map[right] - height_map[idx];
		g._Y = height_map[above] - height_map[idx];

		return g;
	}

	/*
	*	Bilinearly interpolates the given position on the height map.
	*/
	FORCE_INLINE static float BilinearInterpolate(const Parameters &parameters, const uint32 resolution, const float* const RESTRICT height_map, const Vector2<float> position)
	{
		float u, v, ul, ur, ll, lr, ipl_l, ipl_r;
		int32 x_i = (int32)position._X;
		int32 y_i = (int32)position._Y;
		u = position._X - x_i;
		v = position._Y - y_i;
		ul = height_map[y_i * resolution + x_i];
		ur = height_map[y_i * resolution + BaseMath::Minimum<uint32>(x_i + 1, parameters._MaximumBounds._X)];
		ll = height_map[BaseMath::Minimum<uint32>(y_i + 1, parameters._MaximumBounds._Y) * resolution + x_i];
		lr = height_map[BaseMath::Minimum<uint32>(y_i + 1, parameters._MaximumBounds._Y) * resolution + BaseMath::Minimum<uint32>(x_i + 1, parameters._MaximumBounds._X)];
		ipl_l = (1.0f - v) * ul + v * ll;
		ipl_r = (1.0f - v) * ur + v * lr;

		return (1.0f - u) * ipl_l + u * ipl_r;
	}

	/*
	*	Deposits sediment at position `pos` in heighmap `hmap`.
	*	Deposition only affect immediate neighbouring gridpoints
	*	to `pos`.
	*/
	FORCE_INLINE static void deposit(const Parameters &parameters, const uint32 resolution, float* const RESTRICT height_map, const Vector2<float> position, const float amount)
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
	FORCE_INLINE static void erode(const Parameters &parameters, const uint32 resolution, float* const RESTRICT height_map, const Vector2<float> position, const float amount)
	{
		int32 x0 = (int32)position._X - 2;
		int32 y0 = (int32)position._Y - 2;
		int32 x_start = BaseMath::Maximum<int32>(parameters._MinimumBounds._X, x0);
		int32 y_start = BaseMath::Maximum<int32>(parameters._MinimumBounds._Y, y0);
		int32 x_end = BaseMath::Minimum<int32>(parameters._MaximumBounds._X, x0 + 2 * 3 + 1);
		int32 y_end = BaseMath::Minimum<int32>(parameters._MaximumBounds._Y, y0 + 2 * 3 + 1);

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
				float w = BaseMath::Maximum<float32>(0, 2 - distance);
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

};
#endif