//Enable extensions.
#extension GL_EXT_multiview : enable

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_MATRIX;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
    //Write the fragment.
    scene = vec4(vec3(dot(fragment_normal, normalize(vec3(1.0f))) * 0.5f + 0.5f), 1.0f);
}