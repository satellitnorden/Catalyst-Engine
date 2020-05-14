//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) int material_index;
};

//In parameters.
layout (location = 0) in vec3 fragment_current_world_position;
layout (location = 1) in vec3 fragment_previous_world_position;
layout (location = 2) in vec3 fragment_normal;
layout (location = 3) in vec2 fragment_texture_coordinate;

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

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

	//Sample the material properties.
	vec4 materialProperties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

	//Calculate the velocity.
  vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_current_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_previous_world_position);

  //Write the fragments.
  sceneFeatures1 = vec4(albedo, float(material_index) / 255.0f);
  sceneFeatures2 = vec4(fragment_normal, gl_FragCoord.z);
  sceneFeatures3 = materialProperties;
  scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}