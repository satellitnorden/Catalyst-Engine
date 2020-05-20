//Includes.
#include "CatalystMaterialCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 scene_features_4;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenWORLD_TO_CLIP_MATRIX, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenWORLD_TO_CLIP_MATRIX * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void CatalystShaderMain()
{
	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[material_index];

	//Evaluate the material.
	vec4 albedo_thickness;
	vec4 normal_map_displacement;
	vec4 material_properties;
	vec4 opacity;

	EvaluateMaterial(material, fragment_texture_coordinate, albedo_thickness, normal_map_displacement, material_properties, opacity);

	//Construct the tanget space matrix.
	vec3 tangent = vec3(0.0f, 1.0f, 0.0f);
	vec3 bitangent = cross(tangent, fragment_normal);
	vec3 normal = fragment_normal;

	mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

	//Sample the normal map.
	vec3 shading_normal = normal_map_displacement.xyz * 2.0f - 1.0f;
	shading_normal = tangent_space_matrix * shading_normal;
	shading_normal = normalize(shading_normal);

  	//Calculate the velocity.
  	vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position);

    //Write the fragments.
    sceneFeatures1 = albedo_thickness;
    sceneFeatures2 = vec4(shading_normal, gl_FragCoord.z);
    sceneFeatures3 = material_properties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}