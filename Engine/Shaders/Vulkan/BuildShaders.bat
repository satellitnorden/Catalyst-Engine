glslangValidator.exe -V AmbientOcclusionDenoisingFragmentShader.frag -o 		AmbientOcclusionDenoisingFragmentShader.spv
glslangValidator.exe -V BloomIsolationFragmentShader.frag -o 				BloomIsolationFragmentShader.spv
glslangValidator.exe -V DepthOfFieldApplicationFragmentShader.frag -o 			DepthOfFieldApplicationFragmentShader.spv
glslangValidator.exe -V DepthOfFieldBokehFragmentShader.frag -o 			DepthOfFieldBokehFragmentShader.spv
glslangValidator.exe -V DiffuseIrradianceFragmentShader.frag -o 			DiffuseIrradianceFragmentShader.spv
glslangValidator.exe -V DirectLightingRayGenerationShader.rgen -o 			DirectLightingRayGenerationShader.spv
glslangValidator.exe -V FastApproximateAntiAliasingFragmentShader.frag -o 		FastApproximateAntiAliasingFragmentShader.spv
glslangValidator.exe -V FireflyReductionFragmentShader.frag -o 				FireflyReductionFragmentShader.spv
glslangValidator.exe -V LuminanceLightingFragmentShader.frag -o 			LuminanceLightingFragmentShader.spv
glslangValidator.exe -V ModelSceneFeaturesFragmentShader.frag -o 			ModelSceneFeaturesFragmentShader.spv
glslangValidator.exe -V ModelSceneFeaturesVertexShader.vert -o 				ModelSceneFeaturesVertexShader.spv
glslangValidator.exe -V MotionBlurFragmentShader.frag -o 				MotionBlurFragmentShader.spv
glslangValidator.exe -V PassthroughFragmentShader.frag -o 				PassthroughFragmentShader.spv
glslangValidator.exe -V PathTracingDenoisingFragmentShader.frag -o 			PathTracingDenoisingFragmentShader.spv
glslangValidator.exe -V PathTracingRayClosestHitShader.rchit -o 			PathTracingRayClosestHitShader.spv
glslangValidator.exe -V PathTracingRayGenerationShader.rgen -o 				PathTracingRayGenerationShader.spv
glslangValidator.exe -V PathTracingRayMissShader.rmiss -o 				PathTracingRayMissShader.spv
glslangValidator.exe -V PostProcessingFragmentShader.frag -o 				PostProcessingFragmentShader.spv
glslangValidator.exe -V ResampleFragmentShader.frag -o 					ResampleFragmentShader.spv
glslangValidator.exe -V ScreenSpaceAmbientOcclusionFragmentShader.frag -o 		ScreenSpaceAmbientOcclusionFragmentShader.spv
glslangValidator.exe -V ScreenSpaceSpecularIrradianceFragmentShader.frag -o 		ScreenSpaceSpecularIrradianceFragmentShader.spv
glslangValidator.exe -V SeparableBlurFragmentShader.frag -o 				SeparableBlurFragmentShader.spv
glslangValidator.exe -V SpecularIrradianceApplicationFragmentShader.frag -o 		SpecularIrradianceApplicationFragmentShader.spv
glslangValidator.exe -V TemporalAntiAliasingFragmentShader.frag -o 			TemporalAntiAliasingFragmentShader.spv
glslangValidator.exe -V ToneMappingFragmentShader.frag -o 				ToneMappingFragmentShader.spv
glslangValidator.exe -V UserInterfaceFragmentShader.frag -o 				UserInterfaceFragmentShader.spv
glslangValidator.exe -V UserInterfaceVertexShader.vert -o 				UserInterfaceVertexShader.spv
glslangValidator.exe -V VelocityFragmentShader.frag -o 					VelocityFragmentShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv
glslangValidator.exe -V VisibilityRayMissShader.rmiss -o 				VisibilityRayMissShader.spv
glslangValidator.exe -V VolumetricLightingApplicationFragmentShader.frag -o 		VolumetricLightingApplicationFragmentShader.spv
glslangValidator.exe -V VolumetricLightingDenoisingFragmentShader.frag -o 		VolumetricLightingDenoisingFragmentShader.spv
glslangValidator.exe -V VolumetricLightingRayGenerationShader.rgen -o 			VolumetricLightingRayGenerationShader.spv
glslangValidator.exe -V WorldRayClosestHitShader.rchit -o 				WorldRayClosestHitShader.spv
glslangValidator.exe -V WorldRayGenerationShader.rgen -o 				WorldRayGenerationShader.spv
glslangValidator.exe -V WorldRayMissShader.rmiss -o 					WorldRayMissShader.spv

VulkanShaderBuilder.exe

pause