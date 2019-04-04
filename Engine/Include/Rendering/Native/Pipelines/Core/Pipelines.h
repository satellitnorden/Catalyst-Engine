#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AntiAliasingGraphicsPipeline.h>
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Native/Pipelines/GraphicsPipelines/RenderOverrideGraphicsPipeline.h>
#endif
#include <Rendering/Native/Pipelines/GraphicsPipelines/ToneMappingGraphicsPipeline.h>