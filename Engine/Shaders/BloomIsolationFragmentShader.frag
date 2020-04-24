//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Sample the scene texture.
	vec3 scene = texture(scene_texture, fragment_texture_coordinate).rgb;

    //Write the fragment.
    fragment = vec4(scene * bloomIntensity, 1.0f);
}