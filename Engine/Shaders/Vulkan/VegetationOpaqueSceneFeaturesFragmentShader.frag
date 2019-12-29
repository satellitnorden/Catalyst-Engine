//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystTransparency.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 12) int material_index;
    layout (offset = 16) float cutoff_distance_squared;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec3 fragmentPreviousWorldPosition;
layout (location = 4) in vec3 fragmentCurrentWorldPosition;
layout (location = 5) in vec2 fragment_texture_coordinate;
layout (location = 6) in float fragment_distance_squared;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;

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

  //Calculate the opacity.
  float opacity = CalculateDistanceBasedOpacity(fragment_distance_squared, cutoff_distance_squared);

  //Retrieve the random opacity.
  if (opacity < 1.0f)
  {
      //Discard conditionally based on the opacity.
      if (ShouldClip(uint(gl_FragCoord.x), uint(gl_FragCoord.y), opacity))
      {
          discard;
      }
  }

  //Sample the albedo.
  vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

  //Sample the material properties.
  vec4 materialProperties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

  //Calculate the shading normal.
  vec3 shading_normal;

  if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
  {
  	shading_normal = fragmentTangentSpaceMatrix[2];
  }

  else
  {
  	//Sample the normal map.
  	vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).xyz;
  	shading_normal = normal_map * 2.0f - 1.0f;
  	shading_normal = fragmentTangentSpaceMatrix * shading_normal;
  	shading_normal = normalize(shading_normal);
  }

  shading_normal *= gl_FrontFacing ? 1.0f : -1.0f;
  
  //Calculate the velocity.
  vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentCurrentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentPreviousWorldPosition);

  //Write the fragments.
  sceneFeatures1 = vec4(albedo, float(material_index) / 255.0f);
  sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, gl_FragCoord.z);
  sceneFeatures3 = materialProperties;
}