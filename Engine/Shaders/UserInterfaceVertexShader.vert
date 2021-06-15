//Includes.
#include "CatalystUserInterfaceCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
	layout (offset = 0) mat4 TO_WORLD_MATRIX;
	layout (offset = 64) UserInterfaceMaterial MATERIAL;
    layout (offset = 80) vec4 COLOR;
    layout (offset = 96) vec2 MINIMUM;
    layout (offset = 104) vec2 MAXIMUM;
    layout (offset = 112) uint TYPE;
    layout (offset = 116) float WIDTH_RANGE_START;
    layout (offset = 120) float WIDTH_RANGE_END;
    layout (offset = 124) float PRIMITIVE_ASPECT_RATIO;
#else
    layout (offset = 0) UserInterfaceMaterial MATERIAL;
    layout (offset = 16) vec4 COLOR;
    layout (offset = 32) vec2 MINIMUM;
    layout (offset = 40) vec2 MAXIMUM;
    layout (offset = 48) uint TYPE;
    layout (offset = 52) float WIDTH_RANGE_START;
    layout (offset = 56) float WIDTH_RANGE_END;
    layout (offset = 60) float PRIMITIVE_ASPECT_RATIO;
    layout (offset = 64) float TEXT_SMOOTHING_FACTOR;
#endif
};

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;
    
    //Calculate the viewport coordinates.
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
    vec2 viewport_coordinates = vec2(mix(MINIMUM.x, MAXIMUM.x, x), mix(MINIMUM.y, MAXIMUM.y, y));
#else
    vec2 viewport_coordinates = vec2(mix(MINIMUM.x, MAXIMUM.x, x), 1.0f - mix(MINIMUM.y, MAXIMUM.y, y));
#endif

    //Write the position.
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
    gl_Position = WORLD_TO_CLIP_MATRIX * TO_WORLD_MATRIX * vec4(viewport_coordinates.x * 2.0f - 1.0f, viewport_coordinates.y * 2.0f - 1.0f, 0.0f, 1.0f);
#else
    gl_Position = USER_INTERFACE_MATRIX * vec4(viewport_coordinates.x * 2.0f - 1.0f, viewport_coordinates.y * 2.0f - 1.0f, 0.0f, 1.0f);
#endif
} 