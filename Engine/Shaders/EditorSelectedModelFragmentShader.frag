//Layout specification.
layout (early_fragment_tests) in;

//Out parameters.
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
  //Write the fragment.
  scene = vec4(vec3(0.0f, 1.0f, 1.0f), 0.5f);
}