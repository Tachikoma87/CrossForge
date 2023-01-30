#pragma once

#include "CForge/AssetIO/SAssetIO.h"
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "CForge/Graphics/STextureManager.h"

#include "CForge/Graphics/GLWindow.h"
#include "CForge/Graphics/GraphicsUtility.h"
#include "CForge/Graphics/RenderDevice.h"

#include "CForge/Graphics/Lights/DirectionalLight.h"

#include "CForge/Graphics/SceneGraph/SceneGraph.h"
#include "CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "CForge/Graphics/Actors/StaticActor.h"

#include <CForge/Graphics/OpenGLHeader.h>

#include "DekoMesh.hpp"
#include "TreeGenerator.hpp"
#include "RockGenerator.hpp"
#include "GrassGenerator.hpp"
#include "InstanceActor.h"
#include "InstanceSGN.h"

namespace CForge {

	float randomF(float min, float max);

	void placeInstances(int ammount, std::vector<InstanceSGN>& nodes, SGNTransformation& trans, SceneGraph& sceneGraph, InstanceActor& iActor);

	void DecoSetup(void);
}
