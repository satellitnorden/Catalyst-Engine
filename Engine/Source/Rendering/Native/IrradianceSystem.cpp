//Header file.
#include <Rendering/Native/IrradianceSystem.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Noise/HammersleySequence.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post initializes the irradiance system.
*/
void IrradianceSystem::PostInitialize() NOEXCEPT
{
	//Register the uniform data.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("Irradiance"),
		&_UniformData,
		sizeof(IrradianceUniformData)
	);
}

/*
*	Updates the post processing system during the render update phase.
*/
void IrradianceSystem::RenderUpdate() NOEXCEPT
{
	//Define constants.
	constexpr uint32 TOTAL_SAMPLES{ 4'096 * 32 };
	constexpr uint32 POSSIBLE_SAMPLES_PER_SLICE{ TOTAL_SAMPLES / 64 };

	//Update the uniform data.
	for (uint32 sample_index{ 0 }; sample_index < 64; ++sample_index)
	{
		const uint32 _sample_index{ 1 + (POSSIBLE_SAMPLES_PER_SLICE * sample_index) + CatalystRandomMath::RandomIntegerInRange<uint32>(0, POSSIBLE_SAMPLES_PER_SLICE - 1) };

		_UniformData._IrradianceHemisphereDirections[sample_index] = HammersleySequence::CalculateCoordinateHemisphereUniform(_sample_index, TOTAL_SAMPLES);
	}
}