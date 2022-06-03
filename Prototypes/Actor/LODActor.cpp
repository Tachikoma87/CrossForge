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
		for (uint32_t i = 0; i < m_instancedMatRef.size(); i++) {
			delete m_instancedMatRef[i];
		}
	}//Destructor
	
	void LODActor::init(T3DMesh<float>* pMesh, bool isInstanced, bool manualyInstanced) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");
		
		m_VertexArray.init();
		//m_translucent = isTranslucent;
		m_isInstanced = isInstanced;
		m_isManualInstaned = manualyInstanced;
		
		// fetch LOD levels from SLOD if not given
		if (m_LODStages.empty())
			m_LODStages = std::vector<float>(*m_pSLOD->getLevels());

		m_LODMeshes.push_back(pMesh);
		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= VertexUtility::VPROP_UVW;
		if (pMesh->colorCount() > 0) VertexProperties |= VertexUtility::VPROP_COLOR;
		m_VertexProperties.push_back(VertexProperties);
		m_VertexUtility.init(VertexProperties);
		initiateBuffers(0);
		bindLODLevel(0);
		
		std::vector<Eigen::Matrix4f>* m_instLODMats = new std::vector<Eigen::Matrix4f>();
		m_instancedMatRef.push_back(m_instLODMats);
		
		// TODO parallelise generation
		for (uint32_t i = 1; i < m_LODStages.size(); i++) {
			T3DMesh<float>* pLODMesh = new T3DMesh<float>();
			float amount = float(m_LODStages[i]) / m_LODStages[i-1];
			if (amount > 1.0)
				throw CForgeExcept("decimation stages are in wrong order");
			
			MeshDecimator::decimateMesh(m_LODMeshes[i-1], pLODMesh, amount);
			m_LODMeshes.push_back(pLODMesh);
			initiateBuffers(i);
			
			m_instLODMats = new std::vector<Eigen::Matrix4f>();
			m_instancedMatRef.push_back(m_instLODMats);
		}
		
		m_LODMeshes[0]->computeAxisAlignedBoundingBox();
		m_aabb = m_LODMeshes[0]->aabb();
		initAABB();
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh, const std::vector<float>& LODStages, bool isInstanced, bool manualyInstanced) {
		m_LODStages = LODStages;
		init(pMesh, isInstanced, manualyInstanced);
	}//initialize

	void LODActor::initiateBuffers(uint32_t level) {
		auto pMesh = m_LODMeshes[level];

		try {
			uint8_t* pBuffer = nullptr;
			uint32_t BufferSize = 0;
			
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

		m_VertexArray.unbind();
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
		
		//m_VertexArray.clear();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();
		
		m_LODLevel = level;
	}

	uint32_t LODActor::getLODLevel() {
		return m_LODLevel;
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
		
		//if (pRDev->activePass() == RenderDevice::RENDERPASS_LOD) {
			//GLuint queryID;
			//glGenQueries(1, &queryID);
			//glBeginQuery(GL_SAMPLES_PASSED, queryID);
			//
			//glEndQuery(GL_SAMPLES_PASSED);
			//
			//// TODO abfrage im naechsten frame um wartezeit zu ueberbruecken
			//GLint queryState;
			//do {
			//	glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &queryState);
			//} while (!queryState);
			//
			//GLuint pixelCount;
			//glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &pixelCount);
			//glDeleteQueries(1, &queryID);
			//
			////std::cout << pixelCount << "\n";
			//
		//} else {
		
		if (m_isInstanced) {
			// TODO instanced draw call
			for (uint32_t j = 0; j < m_instancedMatRef.size(); j++) {
				if (m_instancedMatRef[j]->size() > 0) {
					pRDev->getInstancedUBO()->setInstances(m_instancedMatRef[j]);

					bindLODLevel(j);
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
						glDrawElementsInstanced(GL_TRIANGLES, i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)), m_instancedMatRef[j]->size());
					}
				}
				m_instancedMatRef[j]->clear();
			}
			if (!m_isManualInstaned)
				m_instancedMatrices.clear();
		} else {
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
		}
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
	
	bool LODActor::renderAABB(class RenderDevice* pRDev, Eigen::Matrix4f sgMat) {
		int32_t level;
		if (m_isManualInstaned) { // all instanced at once
			bool oneWasVisible = false;
			for (uint32_t i = 0; i < m_instancedMatrices.size(); i++) {
				//pRDev->setModelMatrix(mat);
				level = testAABBvis(pRDev);
				if (level >= 0) {
					oneWasVisible = true;
					m_instancedMatRef[level]->push_back(m_instancedMatrices[i]);
				}
			}
			return oneWasVisible;
		}
		else if (m_isInstanced) { // single instance, other instances get added over SG
			level = testAABBvis(pRDev);
			if (level >= 0) {
				m_instancedMatrices.push_back(sgMat);
				m_instancedMatRef[level]->push_back(m_instancedMatrices.at(m_instancedMatrices.size()-1));
			}
		}
		else { // not instanced bind lod immediate
			level = testAABBvis(pRDev);
			if (level != m_LODLevel)
				bindLODLevel(level);
		}
		return level >= 0 ? true : false;
	}
	
	int32_t LODActor::testAABBvis(class RenderDevice* pRDev) {
		
		GLuint queryID;
		glGenQueries(1, &queryID);
		glBeginQuery(GL_SAMPLES_PASSED, queryID);
		
		// render AABB
		pRDev->activeShader(m_AABBshader);
		m_AABBvertArray.bind();
		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, nullptr);
		m_AABBvertArray.unbind();

		glEndQuery(GL_SAMPLES_PASSED);
		// TODO abfrage im naechsten frame um wartezeit zu ueberbruecken
		GLint queryState;
		do {
			glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &queryState);
		} while (!queryState);

		GLuint pixelCount;
		glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &pixelCount);
		glDeleteQueries(1, &queryID);

		//std::cout << pixelCount << "\n";
		if (pixelCount == 0)
			return -1;
		
		// set LOD level based on screen coverage
		float screenCov = float(pixelCount) / m_pSLOD->getResPixAmount();
		//std::cout << screenCov << "\n";
		std::vector<float> LODPerc = m_pSLOD->getLODPercentages();
		uint32_t i = 0;
		while ( i < m_LODStages.size()-1) {
			if (screenCov > LODPerc[i])
				break;
			i++;
		}
		
		return i;
	}
	
	void LODActor::updateAABB() {
		
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
		
		T3DMesh<float>::AABB aabb = m_aabb;
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

	T3DMesh<float>::AABB LODActor::getAABB() {
		return m_aabb;
	}

	void LODActor::initInstancing(bool manualInstanced) {
		m_isManualInstaned = manualInstanced;
	}
	
	void LODActor::addInstance(Eigen::Matrix4f matrix) {
		m_instancedMatrices.push_back(matrix);
	}
	
	void LODActor::setInstanceMatrices(std::vector<Eigen::Matrix4f> matrices) {
		m_instancedMatrices = matrices;
	}

	const std::vector<Eigen::Matrix4f>* LODActor::getInstanceMatrices(uint32_t level) {
		return m_instancedMatRef[level];
	}
}