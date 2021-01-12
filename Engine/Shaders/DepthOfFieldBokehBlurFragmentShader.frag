//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define MAXIMUM_NUMBER_OF_SAMPLES (1024)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float DEPTH_OF_FIELD_FOCUS_DISTANCE;
	layout (offset = 4) float DEPTH_OF_FIELD_SIZE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Converts unit square coordinates to unit disk polar coordinates.
*/
vec2 UnitSquareToUnitDiskPolarCoordinates(float x, float y)
{
    float radius;
    float angle;

    if (abs(x) > abs(y))
    {
        radius = x;
        angle = y / (x + 0.000001f) * 0.785398f;
    }

    else
    {
        radius = y;
        angle = 1.5707963f - (x / (y + 0.000001f) * 0.785398f);
    }

    if (radius < 0.0f)
    {
        radius *= -1.0f;
        angle += PI;
    }

    return vec2(radius, angle);
}

/*
*	Converts unit square coordinates to unit disk coordinates.
*/
vec2 UnitSquareToUnitDiskCoordinates(float x, float y)
{
    vec2 polar_coordinates = UnitSquareToUnitDiskPolarCoordinates(x, y);

    return vec2(polar_coordinates.x * cos(polar_coordinates.y), polar_coordinates.x * sin(polar_coordinates.y));
}

/*
*	Main function.
*/
void CatalystShaderMain()
{
	//Calculate the current view distance.
	float current_view_distance = -(CalculateViewSpacePosition(fragment_texture_coordinate, texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).w).z);

	//Calculate the depth of field weight for this pixel.
	float depth_of_field_weight = min(current_view_distance / DEPTH_OF_FIELD_FOCUS_DISTANCE, 1.0f);

	//Perform all the samples.
	vec3 blurred_scene = vec3(0.0f);
	float total_weight = 0.0f;

	//Calculate the number of samples.
	int number_of_samples = int(mix(1.0f, MAXIMUM_NUMBER_OF_SAMPLES, DEPTH_OF_FIELD_SIZE * depth_of_field_weight));

	for (int Y = -number_of_samples; Y <= number_of_samples; ++Y)
	{
		for (int X = -number_of_samples; X <= number_of_samples; ++X)
		{
			//Calculate the unit square coordinates.
			vec2 unit_disk_coordinate = UnitSquareToUnitDiskCoordinates(float(X) / float(number_of_samples), float(Y) / float(number_of_samples));

			//Scale the unit disk coordinate depending on the depth of field weight/size.
			unit_disk_coordinate *= depth_of_field_weight;
			unit_disk_coordinate *= DEPTH_OF_FIELD_SIZE;

			//Scale the unit disk coordinate depending on the depth of field weight/size.
			unit_disk_coordinate.y *= ASPECT_RATIO;

			//Calculate the sample coordinate.
			vec2 sample_coordinate = fragment_texture_coordinate + unit_disk_coordinate;

			/*
			*	Calculate the sample weight, depending on these criteria;
			*	
			*	1. Is the sample coordinate a valid coordinate?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));

			blurred_scene += texture(sampler2D(RENDER_TARGETS[SCENE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate).rgb * sample_weight;
			total_weight += sample_weight;
		}
	}

	//Normalize the blurred scene.
	blurred_scene = total_weight != 0.0f ? blurred_scene / total_weight : vec3(0.0f);

	//Write the fragment.
	fragment = vec4(blurred_scene, 1.0f);
}