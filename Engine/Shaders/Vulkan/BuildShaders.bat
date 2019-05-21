glslangValidator.exe -V AntiAliasingFragmentShader.frag -o 				AntiAliasingFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxFragmentShader.frag -o 		DebugAxisAlignedBoundingBoxFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxVertexShader.vert -o 		DebugAxisAlignedBoundingBoxVertexShader.spv
glslangValidator.exe -V DebugScreenBoxFragmentShader.frag -o 				DebugScreenBoxFragmentShader.spv
glslangValidator.exe -V DebugScreenBoxVertexShader.vert -o 				DebugScreenBoxVertexShader.spv
glslangValidator.exe -V DenoisingFragmentShader.frag -o 				DenoisingFragmentShader.spv
glslangValidator.exe -V FireflyReductionFragmentShader.frag -o 				FireflyReductionFragmentShader.spv
glslangValidator.exe -V MotionBlurFragmentShader.frag -o 				MotionBlurFragmentShader.spv
glslangValidator.exe -V PassthroughFragmentShader.frag -o 				PassthroughFragmentShader.spv
glslangValidator.exe -V PostProcessingFragmentShader.frag -o 				PostProcessingFragmentShader.spv
glslangValidator.exe -V RadianceIntegrationFragmentShader.frag -o 			RadianceIntegrationFragmentShader.spv
glslangValidator.exe -V TemporalAccumulationFragmentShader.frag -o 			TemporalAccumulationFragmentShader.spv
glslangValidator.exe -V TemporalAccumulationWeightCalculationFragmentShader.frag -o 	TemporalAccumulationWeightCalculationFragmentShader.spv
glslangValidator.exe -V ToneMappingFragmentShader.frag -o 				ToneMappingFragmentShader.spv
glslangValidator.exe -V UserInterfaceFragmentShader.frag -o 				UserInterfaceFragmentShader.spv
glslangValidator.exe -V UserInterfaceVertexShader.vert -o 				UserInterfaceVertexShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv
glslangValidator.exe -V VisibilityRayMissShader.rmiss -o 				VisibilityRayMissShader.spv
glslangValidator.exe -V WorldRayClosestHitShader.rchit -o 				WorldRayClosestHitShader.spv
glslangValidator.exe -V WorldRayGenerationShader.rgen -o 				WorldRayGenerationShader.spv
glslangValidator.exe -V WorldRayMissShader.rmiss -o 					WorldRayMissShader.spv

VulkanShaderBuilder.exe

pause