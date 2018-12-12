glslangValidator.exe -V AboveOceanFragmentShader.frag -o 				AboveOceanFragmentShader.spv
glslangValidator.exe -V AntiAliasingFragmentShader.frag -o 				AntiAliasingFragmentShader.spv
glslangValidator.exe -V BelowOceanFragmentShader.frag -o 				BelowOceanFragmentShader.spv
glslangValidator.exe -V BloomHorizontalFragmentShader.frag -o 				BloomHorizontalFragmentShader.spv
glslangValidator.exe -V BloomVerticalFragmentShader.frag -o 				BloomVerticalFragmentShader.spv
glslangValidator.exe -V BoxBlurFragmentShader.frag -o 					BoxBlurFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxFragmentShader.frag -o 		DebugAxisAlignedBoundingBoxFragmentShader.spv
glslangValidator.exe -V DebugAxisAlignedBoundingBoxVertexShader.vert -o 		DebugAxisAlignedBoundingBoxVertexShader.spv
glslangValidator.exe -V DebugScreenBoxFragmentShader.frag -o 				DebugScreenBoxFragmentShader.spv
glslangValidator.exe -V DebugScreenBoxVertexShader.vert -o 				DebugScreenBoxVertexShader.spv
glslangValidator.exe -V DepthOfFieldFragmentShader.frag -o 				DepthOfFieldFragmentShader.spv
glslangValidator.exe -V DirectionalPhysicalShadowVertexShader.vert -o			DirectionalPhysicalShadowVertexShader.spv
glslangValidator.exe -V DirectionalShadowFragmentShader.frag -o				DirectionalShadowFragmentShader.spv
glslangValidator.exe -V DirectionalSolidVegetationShadowFragmentShader.frag -o		DirectionalSolidVegetationShadowFragmentShader.spv
glslangValidator.exe -V DirectionalSolidVegetationShadowVertexShader.vert -o		DirectionalSolidVegetationShadowVertexShader.spv
glslangValidator.exe -V DirectionalTerrainShadowVertexShader.vert -o			DirectionalTerrainShadowVertexShader.spv
glslangValidator.exe -V GaussianBlurFragmentShader.frag -o				GaussianBlurFragmentShader.spv
glslangValidator.exe -V GrassVegetationColorFragmentShader.frag -o 			GrassVegetationColorFragmentShader.spv
glslangValidator.exe -V GrassVegetationColorVertexShader.vert -o 			GrassVegetationColorVertexShader.spv
glslangValidator.exe -V GrassVegetationDepthFragmentShader.frag -o 			GrassVegetationDepthFragmentShader.spv
glslangValidator.exe -V GrassVegetationDepthVertexShader.vert -o 			GrassVegetationDepthVertexShader.spv
glslangValidator.exe -V HighDetailSolidVegetationFragmentShader.frag -o 		HighDetailSolidVegetationFragmentShader.spv
glslangValidator.exe -V LightingFragmentShader.frag -o 					LightingFragmentShader.spv
glslangValidator.exe -V LowDetailSolidVegetationFragmentShader.frag -o 			LowDetailSolidVegetationFragmentShader.spv
glslangValidator.exe -V MediumDetailSolidVegetationFragmentShader.frag -o 		MediumDetailSolidVegetationFragmentShader.spv
glslangValidator.exe -V OutlineFragmentShader.frag -o 					OutlineFragmentShader.spv
glslangValidator.exe -V ParticleSystemFragmentShader.frag -o 				ParticleSystemFragmentShader.spv
glslangValidator.exe -V ParticleSystemGeometryShader.geom -o 				ParticleSystemGeometryShader.spv
glslangValidator.exe -V ParticleSystemVertexShader.vert -o 				ParticleSystemVertexShader.spv
glslangValidator.exe -V PhysicalFragmentShader.frag -o 					PhysicalFragmentShader.spv
glslangValidator.exe -V PhysicalVertexShader.vert -o 					PhysicalVertexShader.spv
glslangValidator.exe -V ScreenSpaceAmbientOcclusionFragmentShader.frag -o 		ScreenSpaceAmbientOcclusionFragmentShader.spv
glslangValidator.exe -V ShadowMapFragmentShader.frag -o 				ShadowMapFragmentShader.spv
glslangValidator.exe -V SkyFragmentShader.frag -o 					SkyFragmentShader.spv
glslangValidator.exe -V SolidVegetationVertexShader.vert -o 				SolidVegetationVertexShader.spv
glslangValidator.exe -V TerrainColorFragmentShader.frag -o 				TerrainColorFragmentShader.spv
glslangValidator.exe -V TerrainDepthFragmentShader.frag -o 				TerrainDepthFragmentShader.spv
glslangValidator.exe -V TerrainVertexShader.vert -o 					TerrainVertexShader.spv
glslangValidator.exe -V ToneMappingFragmentShader.frag -o 				ToneMappingFragmentShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv
glslangValidator.exe -V VolumetricFogFragmentShader.frag -o 				VolumetricFogFragmentShader.spv
glslangValidator.exe -V WorldPositionFragmentShader.frag -o 				WorldPositionFragmentShader.spv

VulkanShaderBuilder.exe

pause