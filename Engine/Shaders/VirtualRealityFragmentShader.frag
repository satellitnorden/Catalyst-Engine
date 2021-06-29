//Enable extensions.
#extension GL_EXT_multiview : enable

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 COLOR;
};

//Out parameters
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
    //Write the fragment.
    scene = vec4(COLOR, 1.0f);
}