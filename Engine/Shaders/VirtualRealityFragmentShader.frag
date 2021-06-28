//Enable extensions.
#extension GL_EXT_multiview : enable

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
    //Write the fragment.
    scene = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}