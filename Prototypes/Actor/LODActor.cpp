#include <glad/glad.h>
#include "LODActor.h"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Core/SLogger.h"
#include "../MeshDecimate.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"

namespace CForge {
	LODActor::LODActor(void) : IRenderableActor("LODActor", ATYPE_STATIC) {
		m_TypeID = ATYPE_STATIC;
		m_TypeName = "LODActor";
		m_pSLOD = SLOD::instance();
	}//Constructor

	LODActor::~LODActor(void) {
		clear();
		m_pSLOD->release();
	}//Destructor
	
	void LODActor::init(T3DMesh<float>* pMesh, bool isTranslucent) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");
		
		m_translucent = isTranslucent;
		
		// fetch LOD levels from SLOD if not given
		if (m_LODStages.empty())
			m_LODStages = std::vector<float>(*m_pSLOD->getLevels());

		m_LODMeshes.push_back(pMesh);
		initiateBuffers(0);
		bindLODLevel(0);
		
		// TODO parallelise generation
		for (uint32_t i = 1; i < m_LODStages.size(); i++) {
			T3DMesh<float>* pLODMesh = new T3DMesh<float>();
			float amount = float(m_LODStages[i]) / m_LODStages[i-1];
			if (amount > 1.0)
				throw CForgeExcept("decimation stages are in wrong order");
			
			MeshDecimator::decimateMesh(m_LODMeshes[i-1], pLODMesh, amount);
			m_LODMeshes.push_back(pLODMesh);
			initiateBuffers(i);
		}
		
		initAABB();
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh) {
		init(pMesh, true);
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh, bool isTranslucent, const std::vector<float>& LODStages) {
		m_LODStages = LODStages;
		init(pMesh, isTranslucent);
	}//initialize

	void LODActor::initiateBuffers(uint32_t level) {
		auto pMesh = m_LODMeshes[level];
		
		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= VertexUtility::VPROP_UVW;
		if (pMesh->colorCount() > 0) VertexProperties |= VertexUtility::VPROP_COLOR;
		m_VertexProperties.push_back(VertexProperties);

		try {
			uint8_t* pBuffer = nullptr;
			uint32_t BufferSize = 0;
			
			m_VertexUtility.clear();
			m_VertexUtility.init(VertexProperties);
			m_VertexUtility.buildBuffer(pMesh->vertexCount(), (void**)&pBuffer, &BufferSize, pMesh);
						
			m_VertexBuffers.push_back(pBuffer);
			m_VertexBufferSizes.push_back(BufferSize);
			}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during vertex buffer creation!");
			return;
		}

		// build render groups and element array
		try {
			uint8_t* pBuffer = nullptr;
			uint32_t BufferSize = 0;
			RenderGroupUtility* RGU = new RenderGroupUtility();
			RGU->init(pMesh, (void**)&pBuffer, &BufferSize);
			m_RenderGroupUtilities.push_back(RGU);
			
			m_ElementBuffers.push_back(pBuffer);
			m_ElementBufferSizes.push_back(BufferSize);
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during building of index buffer!");
			return;
		}
	}

	void LODActor::bindLODLevel(uint32_t level) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_VertexArray.clear();
		
		try {
			m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, m_VertexBuffers[level], m_VertexBufferSizes[level]);
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during vertex buffer creation!");
			return;
		}

		// build render groups and element array
		try {
			m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, m_ElementBuffers[level], m_ElementBufferSizes[level]);
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during building of index buffer!");
			return;
		}
		
		m_VertexUtility.clear();
		m_VertexUtility.init(m_VertexProperties[level]);
		//m_RenderGroupUtility = *m_RenderGroupUtilities[level]; // TODO this is not necessary?
		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();
		
		m_LODLevel = level;
	}
	
	void LODActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_VertexArray.clear();

		m_VertexUtility.clear();
		m_RenderGroupUtility.clear();

		for (uint32_t i = 0; i < m_LODMeshes.size(); i++) {
			if (i > 0) // do not delete the first mesh, we do not own its reference
				delete m_LODMeshes[i];

			delete m_RenderGroupUtilities[i];
			if (nullptr != m_VertexBuffers[i])
				delete[] m_VertexBuffers[i];
			m_VertexBuffers[i] = nullptr;
			if (nullptr != m_ElementBuffers[i])
				delete[] m_ElementBuffers[i];
			m_ElementBuffers[i] = nullptr;	
		}
	}//Clear

	void LODActor::release(void) {
		delete this;
	}//release

	uint32_t LODActor::materialCount(void) const {
		return m_RenderGroupUtilities[0]->renderGroupCount();
	}//materialCount

	RenderMaterial* LODActor::material(uint32_t Index) {
		if (Index >= m_RenderGroupUtilities[0]->renderGroupCount()) throw IndexOutOfBoundsExcept("Index");
		return &(m_RenderGroupUtilities[0]->renderGroups()[Index]->Material);
	}
	
	void LODActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");
		
		GLuint queryID;
		glGenQueries(1, &queryID);
		glBeginQuery(GL_SAMPLES_PASSED, queryID);
		
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		
		//if (pRDev->activePass() == RenderDevice::RENDERPASS_LOD)
		renderAABB(pRDev);
		//return;
		
		glEndQuery(GL_SAMPLES_PASSED);
		
		// TODO abfrage im naechsten frame um wartezeit zu ueberbruecken
		GLint queryState;
		do {
			glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &queryState);
		} while (!queryState);
		
		GLuint pixelCount;
		glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &pixelCount);
		glDeleteQueries(1, &queryID);
		
		std::cout << pixelCount << "\n";

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		m_VertexArray.bind();
		
		for (auto i : m_RenderGroupUtilities[m_LODLevel]->renderGroups()) {
			if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				pRDev->activeShader(pRDev->shadowPassShader());
			}
			else {
				// TODO Occlusion Culling
				pRDev->activeShader(i->pShader);
				pRDev->activeMaterial(&i->Material);
			}
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
		}//for[all render groups]
		
	}//render
	
	void LODActor::initAABB() {
		
		// create Shader
		const char* vertexShaderSource = "#version 330 core\n"
			"layout(std140) uniform CameraData {\n"
			"	mat4 ViewMatrix;\n"
			"	mat4 ProjectionMatrix;\n"
			"	vec4 Position;\n"
			"} Camera;\n"
			"layout(std140) uniform ModelData {\n"
			"	mat4 ModelMatrix;\n"
			"};\n"
			"in vec3 VertPosition;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * vec4(VertPosition.x, VertPosition.y, VertPosition.z, 1.0);\n"
			"}\0";
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0);\n"
			"}\n\0";

		SShaderManager* shaderManager = SShaderManager::instance();
		uint8_t ConfigOptions = 0;
		ConfigOptions |= ShaderCode::CONF_VERTEXCOLORS;

		ShaderCode* pVertC = shaderManager->createShaderCode(vertexShaderSource, "330", ConfigOptions, "highp", "highp");
		ShaderCode* pFragC = shaderManager->createShaderCode(fragmentShaderSource, "330", ConfigOptions, "highp", "highp");
		std::vector<ShaderCode*> VSSources;
		std::vector<ShaderCode*> FSSources;
		VSSources.push_back(pVertC);
		FSSources.push_back(pFragC);
		std::string buildShaderError;

		m_AABBshader = shaderManager->buildShader(&VSSources, &FSSources, &buildShaderError);
		//std::cout << buildShaderError << "\n";
		shaderManager->release();
		
		// bind AABB to buffer
		updateAABB();
	}
	
	void LODActor::renderAABB(RenderDevice* pRDev) {
		
		pRDev->activeShader(m_AABBshader);
		m_AABBvertArray.bind();
		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, nullptr);
		m_AABBvertArray.unbind();
	}
	
	void LODActor::updateAABB() {
		m_LODMeshes[0]->computeAxisAlignedBoundingBox(); // TODO unwanted side effect?
		T3DMesh<float>::AABB aabb = m_LODMeshes[0]->aabb();
		
		//GLfloat vertices[] = { // Triangle Cube
		//	aabb.Min.x(), aabb.Min.y(), aabb.Max.z(),
		//	aabb.Min.x(), aabb.Max.y(), aabb.Max.z(),
		//	aabb.Min.x(), aabb.Min.y(), aabb.Min.z(),
		//	aabb.Min.x(), aabb.Max.y(), aabb.Min.z(),
		//	aabb.Max.x(), aabb.Min.y(), aabb.Max.z(),
		//	aabb.Max.x(), aabb.Max.y(), aabb.Max.z(),
		//	aabb.Max.x(), aabb.Min.y(), aabb.Min.z(),
		//	aabb.Max.x(), aabb.Max.y(), aabb.Min.z()
		//};
		//GLuint elements[] = {
		//1, 2, 0,
		//3, 6, 2,
		//7, 4, 6,
		//5, 0, 4,
		//6, 0, 2,
		//3, 5, 7,
		//1, 3, 2,
		//3, 7, 6,
		//7, 5, 4,
		//5, 1, 0,
		//6, 4, 0,
		//3, 1, 5
		//};
		
		GLfloat vertices[] = { //Triangle Strip Cube
			aabb.Min.x(), aabb.Min.y(), aabb.Min.z(),
			aabb.Max.x(), aabb.Min.y(), aabb.Min.z(),
			aabb.Min.x(), aabb.Max.y(), aabb.Min.z(),
			aabb.Max.x(), aabb.Max.y(), aabb.Min.z(),
			aabb.Min.x(), aabb.Min.y(), aabb.Max.z(),
			aabb.Max.x(), aabb.Min.y(), aabb.Max.z(),
			aabb.Max.x(), aabb.Max.y(), aabb.Max.z(),
			aabb.Min.x(), aabb.Max.y(), aabb.Max.z()
		};
		GLuint elements[] = {
			3,2,6,
			7,4,2,
			0,3,1,
			6,5,4,
			1,0
		};

		m_AABBvertArray.clear();
		m_AABBvertArray.init();
		m_AABBvertArray.bind();

		m_AABBvertBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, vertices, 24 * sizeof(GLfloat));
		m_AABBindexBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, elements, 14 * sizeof(GLuint));
		m_AABBvertBuffer.bind();
		m_AABBindexBuffer.bind();

		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		m_AABBvertArray.unbind();
	}
	
	bool LODActor::isTranslucent() {
		return m_translucent;
	}
}