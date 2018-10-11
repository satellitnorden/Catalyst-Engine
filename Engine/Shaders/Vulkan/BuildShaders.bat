glslangValidator.exe -V AboveOceanFragmentShader.frag -o 				AboveOceanFragmentShader.spv
glslangValidator.exe -V BelowOceanFragmentShader.frag -o 				BelowOceanFragmentShader.spv
glslangValidator.exe -V BloomFragmentShader.frag -o 					BloomFragmentShader.spv
glslangValidator.exe -V BoxBlurFragmentShader.frag -o 					BoxBlurFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxFragmentShader.frag -o 		DebugAxisAlignedBoundingBoxFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxVertexShader.vert -o 		DebugAxisAlignedBoundingBoxVertexShader.spv
glslangValidator.exe -V DebugScreenBoxFragmentShader.frag -o 				DebugScreenBoxFragmentShader.spv
glslangValidator.exe -V DebugScreenBoxVertexShader.vert -o 				DebugScreenBoxVertexShader.spv
glslangValidator.exe -V DirectionalPhysicalShadowVertexShader.vert -o			DirectionalPhysicalShadowVertexShader.spv
glslangValidator.exe -V DirectionalShadowFragmentShader.frag -o				DirectionalShadowFragmentShader.spv
glslangValidator.exe -V DirectionalShadowInstancedPhysicalVertexShader.vert -o		DirectionalShadowInstancedPhysicalVertexShader.spv
glslangValidator.exe -V DirectionalTerrainShadowVertexShader.vert -o			DirectionalTerrainShadowVertexShader.spv
glslangValidator.exe -V GaussianBlurFragmentShader.frag -o				GaussianBlurFragmentShader.spv
glslangValidator.exe -V GrassVegetationColorFragmentShader.frag -o 			GrassVegetationColorFragmentShader.spv
glslangValidator.exe -V GrassVegetationColorVertexShader.vert -o 			GrassVegetationColorVertexShader.spv
glslangValidator.exe -V GrassVegetationDepthFragmentShader.frag -o 			GrassVegetationDepthFragmentShader.spv
glslangValidator.exe -V GrassVegetationDepthVertexShader.vert -o 			GrassVegetationDepthVertexShader.spv
glslangValidator.exe -V HighDetailTerrainTessellationControlShader.tesc -o 		HighDetailTerrainTessellationControlShader.spv
glslangValidator.exe -V HighDetailTerrainTessellationEvaluationShader.tese -o 		HighDetailTerrainTessellationEvaluationShader.spv
glslangValidator.exe -V HighDetailTerrainVertexShader.vert -o 				HighDetailTerrainVertexShader.spv
glslangValidator.exe -V LightingFragmentShader.frag -o 					LightingFragmentShader.spv
glslangValidator.exe -V LowDetailTerrainVertexShader.vert -o 				LowDetailTerrainVertexShader.spv
glslangValidator.exe -V OutlineFragmentShader.frag -o 					OutlineFragmentShader.spv
glslangValidator.exe -V ParticleSystemFragmentShader.frag -o 				ParticleSystemFragmentShader.spv
glslangValidator.exe -V ParticleSystemGeometryShader.geom -o 				ParticleSystemGeometryShader.spv
glslangValidator.exe -V ParticleSystemVertexShader.vert -o 				ParticleSystemVertexShader.spv
glslangValidator.exe -V PhysicalFragmentShader.frag -o 					PhysicalFragmentShader.spv
glslangValidator.exe -V PhysicalVertexShader.vert -o 					PhysicalVertexShader.spv
glslangValidator.exe -V PostProcessingBloomFragmentShader.frag -o 			PostProcessingBloomFragmentShader.spv
glslangValidator.exe -V PostProcessingFragmentShader.frag -o 				PostProcessingFragmentShader.spv
glslangValidator.exe -V ScreenSpaceAmbientOcclusionFragmentShader.frag -o 		ScreenSpaceAmbientOcclusionFragmentShader.spv
glslangValidator.exe -V ShadowMapFragmentShader.frag -o 				ShadowMapFragmentShader.spv
glslangValidator.exe -V SkyFragmentShader.frag -o 					SkyFragmentShader.spv
glslangValidator.exe -V SolidVegetationVertexShader.vert -o 				SolidVegetationVertexShader.spv
glslangValidator.exe -V TerrainColorFragmentShader.frag -o 				TerrainColorFragmentShader.spv
glslangValidator.exe -V TerrainDepthFragmentShader.frag -o 				TerrainDepthFragmentShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv
glslangValidator.exe -V VolumetricFogFragmentShader.frag -o 				VolumetricFogFragmentShader.spv

VulkanShaderBuilder.exe

pause