//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DEPTH_OF_FIELD_BLUR_START_AND_END (2) //The final number of taps will be ((N * N + 1) * (N * N + 1)).

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

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Maps unit square coordinates to unit disk polar coordinates.
*/
vec2 UnitSquareToUnitDiskPolarMapping(vec2 unit_square)
{
	#define PI_OVER_2 (1.5707963f)
	#define PI_OVER_4 (0.785398f)
	#define EPSILON (0.000001f)

    float radius, angle;

    if (abs(unit_square.x) > abs(unit_square.y))
    { 
        radius = unit_square.x;
        angle = unit_square.y / (unit_square.x + EPSILON) * PI_OVER_4;
    }

    else
    {
        radius = unit_square.y;
        angle = PI_OVER_2 - (unit_square.x / (unit_square.y + EPSILON) * PI_OVER_4);
    }

    if (radius < 0.0f)
    {
        radius *= -1.0f;
        angle += PI;
    }

    return vec2(radius, angle);

    #undef PI_OVER_2
    #undef PI_OVER_4
    #undef EPSILON
}

/*
*	Maps unit square coordinates to unit disk coordinates.
*/
vec2 UnitSquareToUnitDiskMapping(vec2 unit_square)
{
    vec2 polar_coordinates = UnitSquareToUnitDiskPolarMapping(unit_square);

    return vec2(polar_coordinates.x * cos(polar_coordinates.y), polar_coordinates.x * sin(polar_coordinates.y));
}

void main()
{
	//Calculate the hit distance.
	float hit_distance = length(CalculateWorldPosition(fragment_texture_coordinate, texture(scene_features_2_texture, fragment_texture_coordinate).w) - PERCEIVER_WORLD_POSITION);

	//Calculate the depth of field weight for this pixel.
	float depth_of_field_weight = min(hit_distance / DEPTH_OF_FIELD_FOCUS_DISTANCE, 1.0f);

	//Perform all the samples.
	vec3 blurred_scene = vec3(0.0f);
	float total_weight = 0.0f;

	for (int X = -DEPTH_OF_FIELD_BLUR_START_AND_END; X <= DEPTH_OF_FIELD_BLUR_START_AND_END; ++X)
	{
		for (int Y = -DEPTH_OF_FIELD_BLUR_START_AND_END; Y <= DEPTH_OF_FIELD_BLUR_START_AND_END; ++Y)
		{
			//Calculate the unit square coordinate.
			vec2 unit_square_coordinate = vec2(float(X) / DEPTH_OF_FIELD_BLUR_START_AND_END, float(Y) / DEPTH_OF_FIELD_BLUR_START_AND_END);

			//Calculate the unit square coordinates.
			vec2 unit_disk_coordinate = UnitSquareToUnitDiskMapping(unit_square_coordinate);

			//Scale the unit disk coordinate depending on the depth of field weight/size.
			unit_disk_coordinate *= depth_of_field_weight;
			unit_disk_coordinate *= DEPTH_OF_FIELD_SIZE;

			//Scale the unit disk coordinate depending on the depth of field weight/size.
			unit_disk_coordinate.x *= ASPECT_RATIO;

			//Calculate the sample coordinate.
			vec2 sample_coordinate = fragment_texture_coordinate + unit_disk_coordinate;

			/*
			*	Calculate the sample weight, depending on these criteria;
			*	
			*	1. Is the sample coordinate a valid coordinate?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));

			blurred_scene += texture(scene_texture, sample_coordinate).rgb * sample_weight;
			total_weight += sample_weight;
		}
	}

	//Normalize the blurred scene.
	blurred_scene = total_weight != 0.0f ? blurred_scene / total_weight : vec3(0.0f);

	//Write the fragment.
	fragment = vec4(blurred_scene, 1.0f);
}