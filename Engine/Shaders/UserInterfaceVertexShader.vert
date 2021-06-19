//Includes.
#include "CatalystUserInterfaceCore.glsl"

#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
//User interface uniform data.
layout (std140, set = 1, binding = 0) uniform UserInterfaceUniformData
{
	layout (offset = 0) mat4 TO_WORLD_MATRIX;
	layout (offset = 64) vec3 NORMAL;
	layout (offset = 80) vec2 SCALE;
	layout (offset = 88) float ROUGHNESS;
	layout (offset = 92) float METALLIC;
	layout (offset = 96) float AMBIENT_OCCLUSION;
	layout (offset = 100) float EMISSIVE_MULTIPLIER;
};
#endif

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) UserInterfaceMaterial MATERIAL;
    layout (offset = 16) vec4 COLOR;
    layout (offset = 32) vec2 MINIMUM;
    layout (offset = 40) vec2 MAXIMUM;
    layout (offset = 48) uint TYPE;
    layout (offset = 52) float WIDTH_RANGE_START;
    layout (offset = 56) float WIDTH_RANGE_END;
    layout (offset = 60) float PRIMITIVE_ASPECT_RATIO;
    layout (offset = 64) float TEXT_SMOOTHING_FACTOR;
};

//Out parameters.
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
layout (location = 0) out vec3 fragment_world_position;
layout (location = 1) out vec2 fragment_texture_coordinate;
#else
layout (location = 0) out vec2 fragment_texture_coordinate;
#endif

void CatalystShaderMain()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;
    
    
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
    //Calculate the world coordinates.
    vec2 world_coordinates = vec2(mix(MINIMUM.x, MAXIMUM.x, x), mix(MINIMUM.y, MAXIMUM.y, y));
    world_coordinates -= 0.5f;
    world_coordinates *= SCALE;

    //Calculate the world position.
    fragment_world_position = vec3(TO_WORLD_MATRIX * vec4(world_coordinates, 0.0f, 1.0f));

    //Write the position.
    gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);
#else
    //Calculate the viewport coordinates.
    vec2 viewport_coordinates = vec2(mix(MINIMUM.x, MAXIMUM.x, x), 1.0f - mix(MINIMUM.y, MAXIMUM.y, y));

    //Write the position.
    gl_Position = USER_INTERFACE_MATRIX * vec4(viewport_coordinates.x * 2.0f - 1.0f, viewport_coordinates.y * 2.0f - 1.0f, 0.0f, 1.0f);
#endif
} 