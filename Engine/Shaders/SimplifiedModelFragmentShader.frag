//Includes.
#include "CatalystMaterialCore.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_MATRIX;
    layout (offset = 64) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 fragment_normal;
layout (location = 1) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene;

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

	//Write the fragment.
	scene = vec4(albedo_thickness.rgb * (dot(fragment_normal, normalize(vec3(-1.0f, -1.0f, -1.0f))) * 0.5f + 0.5f), 1.0f);
}