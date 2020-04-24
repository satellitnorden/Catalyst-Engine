//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sourceTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
    //Write the fragment.
    fragment = texture(sourceTexture, fragmentTextureCoordinate);
}