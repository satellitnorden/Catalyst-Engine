glslangValidator.exe -V CubeMapVertexShader.vert -o 					CubeMapVertexShader.spv
glslangValidator.exe -V DirectionalPhysicalShadowVertexShader.vert -o			DirectionalPhysicalShadowVertexShader.spv
glslangValidator.exe -V DirectionalShadowFragmentShader.frag -o				DirectionalShadowFragmentShader.spv
glslangValidator.exe -V DirectionalShadowInstancedPhysicalVertexShader.vert -o		DirectionalShadowInstancedPhysicalVertexShader.spv
glslangValidator.exe -V DirectionalShadowTerrainTessellationEvaluationShader.tese -o	DirectionalShadowTerrainTessellationEvaluationShader.spv
glslangValidator.exe -V InstancedPhysicalVertexShader.vert -o 				InstancedPhysicalVertexShader.spv
glslangValidator.exe -V LightingFragmentShader.frag -o 					LightingFragmentShader.spv
glslangValidator.exe -V OceanFragmentShader.frag -o 					OceanFragmentShader.spv
glslangValidator.exe -V ParticleSystemFragmentShader.frag -o 				ParticleSystemFragmentShader.spv
glslangValidator.exe -V ParticleSystemGeometryShader.geom -o 				ParticleSystemGeometryShader.spv
glslangValidator.exe -V ParticleSystemVertexShader.vert -o 				ParticleSystemVertexShader.spv
glslangValidator.exe -V PhysicalFragmentShader.frag -o 					PhysicalFragmentShader.spv
glslangValidator.exe -V PhysicalVertexShader.vert -o 					PhysicalVertexShader.spv
glslangValidator.exe -V PostProcessingFragmentShader.frag -o 				PostProcessingFragmentShader.spv
glslangValidator.exe -V ShadowMapBlurFragmentShader.frag -o 				ShadowMapBlurFragmentShader.spv
glslangValidator.exe -V ShadowMapFragmentShader.frag -o 				ShadowMapFragmentShader.spv
glslangValidator.exe -V SkyFragmentShader.frag -o 					SkyFragmentShader.spv
glslangValidator.exe -V TerrainSceneBufferFragmentShader.frag -o 			TerrainSceneBufferFragmentShader.spv
glslangValidator.exe -V TerrainSceneBufferTessellationControlShader.tesc -o 		TerrainSceneBufferTessellationControlShader.spv
glslangValidator.exe -V TerrainSceneBufferTessellationEvaluationShader.tese -o 		TerrainSceneBufferTessellationEvaluationShader.spv
glslangValidator.exe -V TerrainVertexShader.vert -o 					TerrainVertexShader.spv
glslangValidator.exe -V VegetationFragmentShader.frag -o 				VegetationFragmentShader.spv
glslangValidator.exe -V VegetationGeometryShader.geom -o 				VegetationGeometryShader.spv
glslangValidator.exe -V VegetationVertexShader.vert -o 					VegetationVertexShader.spv
glslangValidator.exe -V ViewportVertexShader.vert -o 					ViewportVertexShader.spv

VulkanShaderBuilder.exe

pause