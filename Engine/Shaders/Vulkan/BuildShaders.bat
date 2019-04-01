glslangValidator.exe -V AntiAliasingFragmentShader.frag -o 				AntiAliasingFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxFragmentShader.frag -o 		DebugAxisAlignedBoundingBoxFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxVertexShader.vert -o 		DebugAxisAlignedBoundingBoxVertexShader.spv
glslangValidator.exe -V DebugScreenBoxFragmentShader.frag -o 				DebugScreenBoxFragmentShader.spv
glslangValidator.exe -V DebugScreenBoxVertexShader.vert -o 				DebugScreenBoxVertexShader.spv
glslangValidator.exe -V PassthroughFragmentShader.frag -o 				PassthroughFragmentShader.spv
glslangValidator.exe -V ToneMappingFragmentShader.frag -o 				ToneMappingFragmentShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv

VulkanShaderBuilder.exe



Copy CatalystShaderCollection.csc ..\..\..\..\Clairvoyant\Resources\Final\CatalystShaderCollection.csc
Copy CatalystShaderCollection.csc ..\..\..\..\Path-Tracing-Prototype\Resources\Final\CatalystShaderCollection.csc
Copy CatalystShaderCollection.csc ..\..\..\..\Playground\Resources\Final\CatalystShaderCollection.csc

pause