//Includes.
#include "CatalystMaterialCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uvec2 RESOLUTION;
	layout (offset = 8) vec2 RESOLUTION_RECIPROCAL;
	layout (offset = 16) float DISTANCE_BETWEEN_BLADES;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec3 fragment_albedo;
layout (location = 2) in vec3 fragment_normal;
layout (location = 3) in vec4 fragment_material_properties;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;
layout (location = 4) out vec4 scene;

void CatalystShaderMain()
{
	//Flip the normal, if necessary.
	vec3 normal = gl_FrontFacing ? fragment_normal : -fragment_normal;

	//Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position);

	//Write the fragments.
	scene_features_1 = vec4(fragment_albedo, 0.0f);
	scene_features_2 = vec4(normal, gl_FragCoord.z);
	scene_features_3 = fragment_material_properties;
	scene_features_4 = vec4(velocity, 0.0f, 0.0f);
	scene = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}