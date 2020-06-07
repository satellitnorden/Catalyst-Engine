//Includes.
#include "CatalystMaterialCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 64) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

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

	//Conditionally discard.
  if (opacity.r < 0.5f)
  {
    discard;
  }
}