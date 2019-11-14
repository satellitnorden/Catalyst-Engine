#if !defined(CATALYST_LIGHTING_DATA)
#define CATALYST_LIGHTING_DATA

//Constants.
#define MAXIMUM_NUMBER_OF_LIGHTS (8)

//Light struct definition.
struct Light
{
  int type;
  vec3 position_or_direction;
	vec3 luminance;
	float size;
};

//Light uniform data.
layout (std140, set = 2, binding = 0) uniform LightUniformData
{
  layout (offset = 0) int numberOfLights;
  layout (offset = 16) vec4[MAXIMUM_NUMBER_OF_LIGHTS * 2] lightData;
};

/*
*	Unpacks the light at the given index.
*/
Light UnpackLight(uint index)
{
	Light light;

  vec4 light_data_1 = lightData[index * 2 + 0];
  vec4 light_data_2 = lightData[index * 2 + 1];

  light.type = floatBitsToInt(light_data_2.z);
  light.position_or_direction = light_data_1.xyz;
  light.luminance = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  light.size = light_data_2.w;

  return light;
}

#endif