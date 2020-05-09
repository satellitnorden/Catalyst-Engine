//Layout specification.
layout (early_fragment_tests) in;

//Out parameters.
layout (location = 0) out vec4 shadow_map;

void CatalystShaderMain()
{
	//Write the fragment.
	shadow_map = vec4(gl_FragCoord.z, 0.0f, 0.0f, 0.0f);
}