#pragma once
#include <CForge/Graphics/OpenGLHeader.h>

#include <iostream>
#include "../../../CForge/Graphics/Actors/IRenderableActor.h"
#include "../../../CForge/AssetIO/T3DMesh.hpp"
#include "../../../CForge/Graphics/GLBuffer.h"
#include "../../../CForge/Graphics/GLVertexArray.h"
#include "../../../CForge/Graphics/Shader/GLShader.h"
#include "../../../CForge/Graphics/RenderMaterial.h"
#include "InstanceVertexUtility.h"
#include "../../../CForge/Graphics/Actors/RenderGroupUtility.h"
#include "../../../CForge/Graphics/RenderDevice.h"
#include "../../../CForge/Graphics/GraphicsUtility.h"
#include "../../../CForge/Core/SLogger.h"


namespace CForge {
	class CFORGE_API InstanceActor: public IRenderableActor {
	public:
		InstanceActor(void);
		~InstanceActor(void);

		void init(const T3DMesh<float>* pMesh);
		void clear(void);
		void release(void);
		void render(RenderDevice* pRDev);

		uint32_t materialCount(void)const;
		RenderMaterial* material(uint32_t Index);
		InstanceVertexUtility m_InstanceVertexUtility;
		void setBufferData(void);
		void addInstance(Matrix4f matrix);
		void clearInstances();

		int instanceOffset;
		int instanceAmmount;
		std::vector<Matrix4f> transMatrixes;
		bool firstInit = true;
	};

}
