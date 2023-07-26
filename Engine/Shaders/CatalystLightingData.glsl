#if !defined(CATALYST_LIGHTING_DATA)
#define CATALYST_LIGHTING_DATA

//Constants.
#define MAXIMUM_NUMBER_OF_LIGHTS (8)

//Light struct definition.
struct Light
{
	vec3 position_or_direction;
	vec3 transform_data_2;
	vec3 color;
	uint light_type;
	uint light_properties;
	float intensity;
	float radius;
	float size;
};

//Light uniform data.
layout (std140, set = CATALYST_LIGHTING_DATA_SET_INDEX, binding = 0) uniform LIGHT_UNIFORM_DATA_BUFFER
{
	layout (offset = 0) uint NUMBER_OF_LIGHTS;
	layout (offset = 4) uint MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS;
};
layout (set = CATALYST_LIGHTING_DATA_SET_INDEX, binding = 1) buffer LIGHT_DATA_BUFFER
{
	layout (offset = 0) vec4[] LIGHT_DATA;
};

/*
*	Unpacks the light at the given index.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 light_data_1 = LIGHT_DATA[index * 4 + 0];
  	vec4 light_data_2 = LIGHT_DATA[index * 4 + 1];
  	vec4 light_data_3 = LIGHT_DATA[index * 4 + 2];
  	vec4 light_data_4 = LIGHT_DATA[index * 4 + 3];

  	light.position_or_direction = vec3(light_data_1.x, light_data_1.y, light_data_1.z);
  	light.transform_data_2 = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  	light.color = vec3(light_data_2.z, light_data_2.w, light_data_3.x);
  	light.light_type = floatBitsToUint(light_data_3.y);
  	light.light_properties = floatBitsToUint(light_data_3.z);
  	light.intensity = light_data_3.w;
  	light.radius = light_data_4.x;
  	light.size = light_data_4.y;

	return light;
}

#endif