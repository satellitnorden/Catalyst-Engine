//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int material_index;
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
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void main()
{
	//Retrieve the material.
  	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

	//Sample the material properties.
	vec4 materialProperties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

	//Calculate the velocity.
  vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragment_current_world_position) - CalculateScreenCoordinate(viewMatrixMinusOne, fragment_previous_world_position);

  //Write the fragments.
  sceneFeatures1 = vec4(albedo, float(material_index) / 255.0f);
  sceneFeatures2 = vec4(fragment_normal, gl_FragCoord.z);
  sceneFeatures3 = materialProperties;
  scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}