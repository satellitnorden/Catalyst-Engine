glslangValidator.exe -V AmbientOcclusionDenoisingFragmentShader.frag -o 		AmbientOcclusionDenoisingFragmentShader.spv
glslangValidator.exe -V CompositingFragmentShader.frag -o 				CompositingFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxFragmentShader.frag -o 		DebugAxisAlignedBoundingBoxFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxVertexShader.vert -o 		DebugAxisAlignedBoundingBoxVertexShader.spv
glslangValidator.exe -V DenoisingFragmentShader.frag -o 				DenoisingFragmentShader.spv
glslangValidator.exe -V DiffuseIrradianceRayClosestHitShader.rchit -o 			DiffuseIrradianceRayClosestHitShader.spv
glslangValidator.exe -V DiffuseIrradianceRayGenerationShader.rgen -o 			DiffuseIrradianceRayGenerationShader.spv
glslangValidator.exe -V DiffuseIrradianceRayMissShader.rmiss -o 			DiffuseIrradianceRayMissShader.spv
glslangValidator.exe -V FastApproximateAntiAliasingFragmentShader.frag -o 		FastApproximateAntiAliasingFragmentShader.spv
glslangValidator.exe -V FireflyReductionFragmentShader.frag -o 				FireflyReductionFragmentShader.spv
glslangValidator.exe -V MotionBlurFragmentShader.frag -o 				MotionBlurFragmentShader.spv
glslangValidator.exe -V PassthroughFragmentShader.frag -o 				PassthroughFragmentShader.spv
glslangValidator.exe -V PathTracingDenoisingFragmentShader.frag -o 			PathTracingDenoisingFragmentShader.spv
glslangValidator.exe -V PathTracingRayClosestHitShader.rchit -o 			PathTracingRayClosestHitShader.spv
glslangValidator.exe -V PathTracingRayGenerationShader.rgen -o 				PathTracingRayGenerationShader.spv
glslangValidator.exe -V PathTracingRayMissShader.rmiss -o 				PathTracingRayMissShader.spv
glslangValidator.exe -V PostProcessingFragmentShader.frag -o 				PostProcessingFragmentShader.spv
glslangValidator.exe -V RadianceIntegrationFragmentShader.frag -o 			RadianceIntegrationFragmentShader.spv
glslangValidator.exe -V SceneFeaturesFragmentShader.frag -o 				SceneFeaturesFragmentShader.spv
glslangValidator.exe -V SceneFeaturesVertexShader.vert -o 				SceneFeaturesVertexShader.spv
glslangValidator.exe -V ScreenSpaceAmbientOcclusionFragmentShader.frag -o 		ScreenSpaceAmbientOcclusionFragmentShader.spv
glslangValidator.exe -V ScreenSpaceSpecularIrradianceFragmentShader.frag -o 		ScreenSpaceSpecularIrradianceFragmentShader.spv
glslangValidator.exe -V SimpleDiffuseIrradianceFragmentShader.frag -o 			SimpleDiffuseIrradianceFragmentShader.spv
glslangValidator.exe -V TemporalAccumulationFragmentShader.frag -o 			TemporalAccumulationFragmentShader.spv
glslangValidator.exe -V TemporalAntiAliasingFragmentShader.frag -o 			TemporalAntiAliasingFragmentShader.spv
glslangValidator.exe -V ToneMappingFragmentShader.frag -o 				ToneMappingFragmentShader.spv
glslangValidator.exe -V UserInterfaceFragmentShader.frag -o 				UserInterfaceFragmentShader.spv
glslangValidator.exe -V UserInterfaceVertexShader.vert -o 				UserInterfaceVertexShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv
glslangValidator.exe -V VisibilityRayMissShader.rmiss -o 				VisibilityRayMissShader.spv
glslangValidator.exe -V VolumetricLightingDenoisingFragmentShader.frag -o 		VolumetricLightingDenoisingFragmentShader.spv
glslangValidator.exe -V VolumetricLightingRayGenerationShader.rgen -o 			VolumetricLightingRayGenerationShader.spv
glslangValidator.exe -V WorldRayClosestHitShader.rchit -o 				WorldRayClosestHitShader.spv
glslangValidator.exe -V WorldRayGenerationShader.rgen -o 				WorldRayGenerationShader.spv
glslangValidator.exe -V WorldRayMissShader.rmiss -o 					WorldRayMissShader.spv

VulkanShaderBuilder.exe

pause