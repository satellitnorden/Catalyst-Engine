//Includes.
#include "CatalystMaterialCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) float HALF_WIDTH;
    layout (offset = 20) float WHOLE_WIDTH;
    layout (offset = 24) float HEIGHT;
    layout (offset = 28) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in mat3 fragment_tangent_space_matrix;
layout (location = 3) in vec3 fragment_world_position;
layout (location = 4) in vec3 fragment_normal;
layout (location = 5) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;
layout (location = 4) out vec4 scene;

void CatalystShaderMain()
{
  //Retrieve the material.
  Material material = GLOBAL_MATERIALS[MATERIAL_INDEX];

  //Evaluate the material.
  vec4 albedo_thickness;
  vec4 normal_map_displacement;
  vec4 material_properties;
  vec4 opacity;

  EvaluateMaterial(material, fragment_texture_coordinate, albedo_thickness, normal_map_displacement, material_properties, opacity);

  //Calculate the shading normal.
  vec3 shading_normal = normal_map_displacement.xyz * 2.0f - 1.0f;
  shading_normal = fragment_tangent_space_matrix * shading_normal;
  shading_normal = normalize(shading_normal);

  //Calculate the velocity.
  vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position) - CURRENT_FRAME_JITTER;

  //Write the fragments.
  scene_features_1 = albedo_thickness;
  scene_features_2 = vec4(shading_normal, gl_FragCoord.z);
  scene_features_3 = material_properties;
  scene_features_4 = vec4(velocity, 0.0f, 0.0f);
  scene = vec4(albedo_thickness.rgb * material_properties[3] * material._EmissiveMultiplier, 1.0f);
}