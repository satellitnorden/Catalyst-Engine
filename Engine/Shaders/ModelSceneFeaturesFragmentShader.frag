//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 128) uint material_index;
};

//In parameters.
layout (location = 0) in mat3 fragment_tangent_space_matrix;
layout (location = 3) in vec3 fragment_previous_world_position;
layout (location = 4) in vec3 fragment_current_world_position;
layout (location = 5) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;
layout (location = 4) out vec4 scene;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 given_matrix, vec3 world_position)
{
  vec4 view_space_position = given_matrix * vec4(world_position, 1.0f);
  float view_space_position_w_reciprocal = 1.0f / view_space_position.w;
  view_space_position.xy *= view_space_position_w_reciprocal;

  return view_space_position.xy * 0.5f + 0.5f;
}

void CatalystShaderMain()
{
   	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = fragment_tangent_space_matrix[2];
	}

	else
	{
	 //Sample the normal map.
	 vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).xyz;
	 shading_normal = normal_map * 2.0f - 1.0f;
	 shading_normal = fragment_tangent_space_matrix * shading_normal;
	 shading_normal = normalize(shading_normal);
	}

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_current_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_previous_world_position);

    //Write the fragments.
    scene_features_1 = vec4(albedo, 1.0f);
    scene_features_2 = vec4(shading_normal, gl_FragCoord.z);
    scene_features_3 = material_properties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
    scene = vec4(albedo * material_properties[3] * material.luminance_multiplier, 1.0f);
}