#include <glad/glad.h>
#include "LODActor.h"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Core/SLogger.h"
#include "../MeshDecimate.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../LODHandler.h"
#include <iostream>

#define LOD_RENDERING true
#define SKIP_INSTANCED_QUERIES false
#define CPU_QUERY true

#define CMIX false // use FOV culling before FRUSTUM culling
#define CFOV false // use FOV culling instead of FRUSTUM culling
#define CFOV_ARCCOS false // use slower arccos calculation for FOV culling

#define M_PI 3.1415926535

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
		
		pMesh->computeAxisAlignedBoundingBox();
		m_aabb = pMesh->aabb();
		initAABB();
		
		//m_VertexArray.init();
		//m_translucent = isTranslucent;
		m_isInstanced = isInstanced || manualyInstanced;
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
		
		//if (m_pSLOD->forceLODregeneration)
		//	generateLODModells();
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh, const std::vector<float>& LODStages, bool isInstanced, bool manualyInstanced) {
		m_LODStages = LODStages;
		init(pMesh, isInstanced, manualyInstanced);
	}//initialize
	
	void LODActor::generateLODModells() {

		// TODO parallelise generation
		for (uint32_t i = 1; i < m_LODStages.size(); i++) {
			T3DMesh<float>* pLODMesh = new T3DMesh<float>();
			float amount = float(m_LODStages[i]) / m_LODStages[i-1];
			if (amount > 1.0)
				throw CForgeExcept("decimation stages are in wrong order");

			bool succ = MeshDecimator::decimateMesh(m_LODMeshes[i-1], pLODMesh, amount);
			bool faceless = false;
			for (uint32_t j = 0; j < pLODMesh->submeshCount(); j++) {
				if (pLODMesh->getSubmesh(j)->Faces.size() < 1)
					faceless = true;
			}

			if (!succ || pLODMesh->vertexCount() < 3 || faceless) { // decimation failed due to to few triangles to decimate
				m_LODStages.erase(m_LODStages.begin()+i, m_LODStages.end());
				delete pLODMesh;
				//m_LODMeshes.push_back(nullptr);
				break;
			}
			m_LODMeshes.push_back(pLODMesh);
			pLODMesh->computePerVertexNormals();
			initiateBuffers(i);

			std::vector<Eigen::Matrix4f>* m_instLODMats = new std::vector<Eigen::Matrix4f>();
			m_instancedMatRef.push_back(m_instLODMats);
		}

		calculateLODPercentages();
		
		for (uint32_t i = 1; i < m_LODMeshes.size(); i++) {
			delete m_LODMeshes[i];
			m_LODMeshes[i] = nullptr;
		}
	}
	
	void LODActor::calculateLODPercentages() {
		

#if CPU_QUERY
		// area of Bounding Sphere
		float r = getAABBradius(Eigen::Matrix4f::Identity());
		float side = M_PI*r*r;
#else
		// get surface area of biggest AABB side
		Eigen::Vector3f diag = m_LODMeshes[0]->aabb().diagonal();
		float a = std::max(diag.x(),std::max(diag.y(),diag.z()));
		float b = diag.x()+diag.y()+diag.z()-a-std::min(diag.x(),std::min(diag.y(),diag.z()));
		float side = (a*b);
#endif

		for (uint32_t i = 0; i < m_LODMeshes.size(); i++) {
			
			float triangleSize = LODHandler::getTriSizeInfo(*m_LODMeshes[i],LODHandler::TRI_S_AVG);
			m_LODPercentages.push_back(triangleSize/side);
		}
	}
	
	void LODActor::setLODmeshes(std::vector<T3DMesh<float>*> meshes) {
		for (uint32_t i = 1; i < meshes.size(); i++) {
			m_LODMeshes.push_back(meshes[i]);
			std::vector<Eigen::Matrix4f>* m_instLODMats = new std::vector<Eigen::Matrix4f>();
			m_instancedMatRef.push_back(m_instLODMats);
		}
	}
	
	void LODActor::freeLODMeshes() {
		for (uint32_t i = 0; i < m_LODMeshes.size(); i++) {
			if (i > 0)
				delete m_LODMeshes[i];
			m_LODMeshes[i] = nullptr;
		}
		//m_LODMeshes.clear(); // TODO ?
	}
	
	void LODActor::initiateLODBuffers() {
		for (uint32_t i = 1; i < m_LODMeshes.size(); i++) {
			initiateBuffers(i);
		}
	}

	void LODActor::initiateBuffers(uint32_t level) {
		auto pMesh = m_LODMeshes[level];

		if (storeOnVRAM) {
			m_pVertexArrays.push_back(new GLVertexArray());
			m_pVertexArrays.back()->init();
			m_pVertexBuffers.push_back(new GLBuffer());
			m_pElementBuffers.push_back(new GLBuffer());
			try {
				uint8_t* pBuffer = nullptr;
				uint32_t BufferSize = 0;
				m_VertexUtility.buildBuffer(pMesh->vertexCount(), (void**)&pBuffer, &BufferSize, pMesh);
				m_pVertexBuffers.back()->init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
				if(nullptr != pBuffer) delete[] pBuffer;
				pBuffer = nullptr;
				BufferSize = 0;
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
				m_pElementBuffers.back()->init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
				m_ElementBufferSizes.push_back(BufferSize);
				if(nullptr != pBuffer) delete[] pBuffer;
				pBuffer = nullptr;
				BufferSize = 0;
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
				return;
			}
			catch (...) {
				SLogger::log("Unknown exception occurred during building of index buffer!");
				return;
			}
		} else {
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
	}

	void LODActor::bindLODLevel(uint32_t level) { // TODO m_VertexArray
		if (level == m_LODLevel)
			return;
		m_LODLevel = level;

		if (storeOnVRAM) {
			m_pVertexArray = m_pVertexArrays[level];
			m_pVertexArray->bind();
			setBufferData();
			m_pVertexArray->unbind();
		} else {
			m_pVertexArray->unbind();
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

			m_pVertexArray->bind();
			setBufferData();
			m_pVertexArray->unbind();
		}
	}

	uint32_t LODActor::getLODLevel() {
		return m_LODLevel;
	}

	void LODActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_pVertexArray->clear();

		m_VertexUtility.clear();
		m_RenderGroupUtility.clear();

		for (uint32_t i = 0; i < m_RenderGroupUtilities.size(); i++) {
			if (nullptr != m_RenderGroupUtilities[i]) {
				m_RenderGroupUtilities[i]->clear();
				delete m_RenderGroupUtilities[i];
				m_RenderGroupUtilities[i] = nullptr;
			}
		}
		for (uint32_t i = 0; i < m_VertexBuffers.size(); i++) {
			if (nullptr != m_VertexBuffers[i]) {
				delete m_VertexBuffers[i];
				m_VertexBuffers[i] = nullptr;
			}
		}
		for (uint32_t i = 0; i < m_ElementBuffers.size(); i++) {
			if (nullptr != m_ElementBuffers[i]) {
				delete m_ElementBuffers[i];
				m_ElementBuffers[i] = nullptr;
			}
		}

		if (storeOnVRAM) {
			for (uint32_t i = 0; i < m_pVertexArrays.size(); i++) {
				if (nullptr != m_pVertexArrays[i]) {
					delete m_pVertexArrays[i];
					m_pVertexArrays[i] = nullptr;
				}
			}
			for (uint32_t i = 0; i < m_pVertexBuffers.size(); i++) {
				if (nullptr != m_pVertexBuffers[i]) {
					delete m_pVertexBuffers[i];
					m_pVertexBuffers[i] = nullptr;
				}
			}
			for (uint32_t i = 0; i < m_pElementBuffers.size(); i++) {
				if (nullptr != m_pElementBuffers[i]) {
					delete m_pElementBuffers[i];
					m_pElementBuffers[i] = nullptr;
				}
			}
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

		if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW && !m_castShadows) {
			return;
		}
		
		if (!m_faceCulling)
			glDisable(GL_CULL_FACE);
		if (m_isInstanced) {
#if LOD_RENDERING
			for (uint32_t j = 0; j < m_instancedMatRef.size(); j++) {
				if (m_instancedMatRef[j]->size() > 0) {
					
					bindLODLevel(j);
					m_pVertexArray->bind();
					uint32_t maxInstances = pRDev->getInstancedUBO()->getMaxInstanceCount();
					
					for (uint32_t k = 0; k < m_instancedMatRef[j]->size(); k += maxInstances) {
						
						uint32_t range = std::min((k + maxInstances), (uint32_t) m_instancedMatRef[j]->size());
						pRDev->getInstancedUBO()->setInstances(m_instancedMatRef[j], Eigen::Vector2i(k, range));
						
						for (auto i : m_RenderGroupUtilities[m_LODLevel]->renderGroups()) {
							if (i->pShader == nullptr) continue;
							if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
								pRDev->activeShader(pRDev->shadowPassShaderInstanced());
							}
							else {
								pRDev->activeShader(i->pShader);
								pRDev->activeMaterial(&i->Material);
							}
							glDrawElementsInstanced(GL_TRIANGLES, i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)), range-k);
						} //for [render groups]
					} //for [maxInstances]
				} //if [instances exist]
			}
#else
			m_pVertexArray->bind();
			uint32_t maxInstances = pRDev->getInstancedUBO()->getMaxInstanceCount();

			for (uint32_t k = 0; k < m_instancedMatrices.size(); k += maxInstances) {

				uint32_t range = std::min((k + maxInstances), (uint32_t) m_instancedMatrices.size());
				pRDev->getInstancedUBO()->setInstances(&m_instancedMatrices, Eigen::Vector2i(k, range));

				for (auto i : m_RenderGroupUtilities[m_LODLevel]->renderGroups()) {
					if (i->pShader == nullptr) continue;
					if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
						pRDev->activeShader(pRDev->shadowPassShaderInstanced());
					}
					else {
						pRDev->activeShader(i->pShader);
						pRDev->activeMaterial(&i->Material);
					}
					glDrawElementsInstanced(GL_TRIANGLES, i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)), range-k);
				} //for [render groups]
			} //for [maxInstances]
#endif
		} else {
			m_pVertexArray->bind();
			for (auto i : m_RenderGroupUtilities[m_LODLevel]->renderGroups()) {
				if (i->pShader == nullptr) continue;
				if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
					pRDev->activeShader(pRDev->shadowPassShader());
				}
				else {
					pRDev->activeShader(i->pShader);
					pRDev->activeMaterial(&i->Material);
				}
				glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBufferSizes[m_LODLevel] / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
			}//for[all render groups]
		}
		if (!m_faceCulling)
			glEnable(GL_CULL_FACE);
	}//render
	
	void LODActor::initAABB() {
		
		// create Shader
		const char* vertexShaderSource = "#version 420 core\n"
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
		const char* fragmentShaderSource = "#version 420 core\n"
			"layout(early_fragment_tests) in;\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0);\n"
			"}\n\0";

		SShaderManager* shaderManager = SShaderManager::instance();
		uint8_t ConfigOptions = 0;
		ConfigOptions |= ShaderCode::CONF_VERTEXCOLORS;

		ShaderCode* pVertC = shaderManager->createShaderCode(vertexShaderSource, "420", ConfigOptions, "highp", "highp");
		ShaderCode* pFragC = shaderManager->createShaderCode(fragmentShaderSource, "420", ConfigOptions, "highp", "highp");
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
	
	void LODActor::testAABBvis(class RenderDevice* pRDev, Eigen::Matrix4f sgMat) {
		
// 		double t_mean = 0.0;
// 		uint32_t c = 0;
		if (m_isManualInstaned) { // all instanced at once
			for (uint32_t i = 0; i < m_instancedMatrices.size(); i++) {

// 				auto start = std::chrono::steady_clock::now();
				
#if CMIX
				bool res2 = !fovCulling(pRDev, &m_instancedMatrices[i]);
				bool res;
				if (res2)
					res = true;
				else
					res = !frustumCulling(pRDev, &m_instancedMatrices[i]);
#elif CFOV
				bool res2 = !fovCulling(pRDev, &m_instancedMatrices[i]);
				bool res = res2;
#else
				bool res2 = !frustumCulling(pRDev, &m_instancedMatrices[i]);
				bool res = res2;
#endif
	
// 				auto end = std::chrono::steady_clock::now();

// 				long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
// 				double time = 0.000001 * microseconds;
// 				t_mean += microseconds;
				if (res)
					continue;
#if SKIP_INSTANCED_QUERIES
				m_instancedMatRef[0]->push_back(m_instancedMatrices[i]);
				if (!this->isInLODSG()) {
					pRDev->LODSGPushBack(this, Eigen::Matrix4f::Identity());
					this->setLODSG(true);
				}
#else
				pRDev->modelUBO()->modelMatrix(m_instancedMatrices[i]);
				queryAABB(pRDev, m_instancedMatrices[i]);
#endif
				//c++;
				//if (c > 1000) {
				//	std::cout << "t_mean " << t_mean << "\n";
				//	exit(1);
				//}
			}
			//std::cout << m_instancedMatrices.size() << "\n";
			//std::cout << t_mean << "\n";
			
// 			m_pSLOD->CT += t_mean;
// 			m_pSLOD->CTC += m_instancedMatrices.size();
		}
		else if (m_isInstanced) { // single instance, other instances get added over SG
			if (!frustumCulling(pRDev, &sgMat))
				return;
#if SKIP_INSTANCED_QUERIES
			m_instancedMatrices.push_back(sgMat);
			m_instancedMatRef[0]->push_back(m_instancedMatrices.at(m_instancedMatrices.size()-1));
			if (!this->isInLODSG()) {
				pRDev->LODSGPushBack(this, Eigen::Matrix4f::Identity());
				this->setLODSG(true);
			}
#else
			queryAABB(pRDev, sgMat);
#endif
		}
		else {
// 			auto start = std::chrono::steady_clock::now();

#if CMIX
			bool res2 = !fovCulling(pRDev, &sgMat);
			bool res;
			if (res2)
				res = true;
			else
				res = !frustumCulling(pRDev, &sgMat);
#elif CFOV
			bool res2 = !fovCulling(pRDev, &sgMat);
			bool res = res2;
#else
			bool res2 = !frustumCulling(pRDev, &sgMat);
			bool res = res2;
#endif

// 			auto end = std::chrono::steady_clock::now();

// 			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
// 			double time = 0.000001 * microseconds;
// 			t_mean += time;
// 			m_pSLOD->CT += t_mean;
// 			m_pSLOD->CTC += m_instancedMatrices.size();
			if (res)
				return;
			queryAABB(pRDev, sgMat);
		}
	}
	
	void LODActor::queryAABB(RenderDevice* pRDev, Eigen::Matrix4f transform) {

#if CPU_QUERY
		Eigen::Vector3f Translation = Eigen::Vector3f(transform.data()[12], transform.data()[13], transform.data()[14]);
		float aabbRadius = getAABBradius(transform);
		Eigen::Vector3f camPosToObj = Translation+getAABBcenter(transform)-pRDev->activeCamera()->position();
		float BSborderAngle = 2.0*std::asin(aabbRadius/(2.0*camPosToObj.norm()));
		float fov = pRDev->activeCamera()->getFOV();
		float screenCov = BSborderAngle*BSborderAngle/(fov*fov);
		
		RenderDevice::LODQueryContainer container;
		container.pActor = this;
		container.pixelCount = screenCov*m_pSLOD->getResPixAmount();
		container.queryID = 0;
		container.transform = transform;
		pRDev->LODQueryContainerPushBack(container);
#else
		GLuint queryID;
		glGenQueries(1, &queryID);
		glBeginQuery(GL_SAMPLES_PASSED, queryID);
		
		if (!glIsQuery(queryID)) {
			CForgeExcept("query generation failed");
			// fetch current queries if no more are available
			pRDev->fetchQueryResults();
			glGenQueries(1, &queryID);
			glBeginQuery(GL_SAMPLES_PASSED, queryID);
		}
		
		pRDev->LODQueryContainerPushBack(queryID, this, transform);
		
		renderAABB(pRDev);
		glEndQuery(GL_SAMPLES_PASSED);
#endif
	}
	
	void LODActor::evaluateQueryResult(Eigen::Matrix4f mat, GLint pixelCount) {
		int32_t level = 0;
		
		if (pixelCount == 0)
			return;

// 		printf("pixelCount			%d\n", pixelCount);
		// set LOD level based on screen coverage
		float screenCov = float(pixelCount);// / m_pSLOD->getResPixAmount();
// 		printf("screenCov			%f\n", screenCov);
		
		std::vector<float> percentages;
		if (m_LODPercentages.size() < m_LODStages.size())
			percentages = m_pSLOD->getLODPercentages();
		else
			percentages = m_LODPercentages;
		
		float ratio = (float) m_pSLOD->TriangleSize; ///m_pSLOD->getResPixAmount();
		
		while (level < m_LODStages.size()-1) {
			if (screenCov*percentages[level] > ratio)
				break;
			level++;
		}
		
		if (m_isManualInstaned) {
			m_instancedMatRef[level]->push_back(mat);
		}
		else if (m_isInstanced) {
			m_instancedMatrices.push_back(mat);
			m_instancedMatRef[level]->push_back(m_instancedMatrices.at(m_instancedMatrices.size() - 1));
		}
		else { // not instanced bind lod immediately
			if (level != m_LODLevel) {
				//printf("binding LOD level: %d\n", level);
				bindLODLevel(level);
			}
		}
	}
	
	
	void LODActor::renderAABB(class RenderDevice* pRDev) {
		pRDev->activeShader(m_AABBshader);
		m_AABBvertArray.bind();
		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, nullptr);
		m_AABBvertArray.unbind();
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

	void LODActor::clearInstances() {
		m_instancedMatrices.clear();
	}

	void LODActor::changeInstance(uint32_t index, Eigen::Matrix4f mat) {
		m_instancedMatrices[index] = mat;
	}
	
	void LODActor::setInstanceMatrices(std::vector<Eigen::Matrix4f> matrices) {
		m_instancedMatrices = matrices;
	}

	const std::vector<Eigen::Matrix4f>* LODActor::getInstanceMatrices(uint32_t level) {
		return m_instancedMatRef[level];
	}
	
	std::vector<float> LODActor::getLODStages() {
		return m_LODStages;
	}

	void LODActor::setLODStages(std::vector<float> vec) {
		m_LODStages = vec;
	}

	std::vector<float> LODActor::getLODPercentages() {
		return m_LODPercentages;
	}

	inline bool LODActor::fovCulling(RenderDevice* pRDev, const Eigen::Matrix4f* mat) {
#if CFOV_ARCCOS
		{
			Eigen::Vector3f Translation = Eigen::Vector3f(mat->data()[12], mat->data()[13], mat->data()[14]);
		
			float aabbRadius = getAABBradius(*mat);
			Eigen::Vector3f camPosToObj = Translation+getAABBcenter(*mat)-pRDev->activeCamera()->position();
		
			if (camPosToObj.norm()-aabbRadius < 0.0)
				return true;
		
			float BSborderAngle = 2.0*std::asin(aabbRadius/(2.0*camPosToObj.norm()));
		
			float fov = pRDev->activeCamera()->getFOV();
			if (std::acos(camPosToObj.normalized().dot(pRDev->activeCamera()->dir())) - (BSborderAngle) > (fov))
				return false;
			return true;
			//std::cout << "angle: " << std::cos(std::acos(camPosToObj.normalized().dot(pRDev->activeCamera()->dir())) - (BSborderAngle)) << "\n";
		}
#else
		{
			Eigen::Vector3f Translation = Eigen::Vector3f(mat->data()[12], mat->data()[13], mat->data()[14]);

			float aabbRadius = getAABBradius(*mat);
			Eigen::Vector3f camPosToObj = Translation+getAABBcenter(*mat)-pRDev->activeCamera()->position();

			float a = camPosToObj.norm();
			if (a-aabbRadius < 0.0)
				return true;
			float x = 1.0-aabbRadius*aabbRadius/(2.0*a*a);
			float y = std::sqrt(1.0-x*x);


			Eigen::Vector3f von = camPosToObj.normalized();
			Eigen::Vector3f vn = von.cross(pRDev->activeCamera()->dir());
			Eigen::Vector3f pc = vn.cross(von);
			Eigen::Vector3f vec = von*x + pc*y;

			float res = vec.dot(pRDev->activeCamera()->dir());
			float c = von.dot(pRDev->activeCamera()->dir());
// 			std::cout << "alt: " << res << "\n";
			
			float fov = m_pSLOD->getCFOV();
			if (res < fov && c < fov)
				return false;
			return true;
		}
#endif
	}

	// frustum culling reference: https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
	inline bool LODActor::frustumCulling(RenderDevice* pRDev, const Eigen::Matrix4f* mat) {
		Eigen::Vector3f Translation = Eigen::Vector3f(mat->data()[12], mat->data()[13], mat->data()[14]);
		
		// frustum in world space
		const VirtualCamera::Frustum* pFrust = pRDev->activeCamera()->getFrustum();
		
		Eigen::Affine3f aff(*mat);
		
		// bounding box in world space
		Eigen::Vector3f min = aff * m_aabb.Min;
		Eigen::Vector3f max = aff * m_aabb.Max;
		Eigen::Vector3f c = 0.5*min+0.5*max;
		Eigen::Vector3f e = max-c;
		
		Eigen::Vector3f ri = mat->col(0).head<3>().normalized()*e[0];
		Eigen::Vector3f up = mat->col(1).head<3>().normalized()*e[1];
		Eigen::Vector3f fd = mat->col(2).head<3>().normalized()*e[2];
		
		// restore AABB due to Object Space AABB -> World Space BB
		float Ii = abs(Eigen::Vector3f(1.0f,0.0f,0.0f).dot(ri))
		         + abs(Eigen::Vector3f(1.0f,0.0f,0.0f).dot(up))
		         + abs(Eigen::Vector3f(1.0f,0.0f,0.0f).dot(fd));
		
		float Ij = abs(Eigen::Vector3f(0.0f,1.0f,0.0f).dot(ri))
		         + abs(Eigen::Vector3f(0.0f,1.0f,0.0f).dot(up))
		         + abs(Eigen::Vector3f(0.0f,1.0f,0.0f).dot(fd));
		
		float Ik = abs(Eigen::Vector3f(0.0f,0.0f,1.0f).dot(ri))
		         + abs(Eigen::Vector3f(0.0f,0.0f,1.0f).dot(up))
		         + abs(Eigen::Vector3f(0.0f,0.0f,1.0f).dot(fd));
		
		max = c+Eigen::Vector3f(Ii,Ij,Ik);
		min = c-Eigen::Vector3f(Ii,Ij,Ik);
		T3DMesh<float>::AABB sclAABB = {min,max};
		
		return AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_NEAR])
		    && AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_LEFT])
		    && AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_RIGHT])
		    && AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_BOTTOM])
		    && AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_TOP])
		    && AABBonPlan(&sclAABB, &pFrust->plan[VirtualCamera::FRUSTUMPLANE_FAR]);
	}
	
	// algorithm from https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	inline bool LODActor::AABBonPlan(const T3DMesh<float>::AABB* aabb, const VirtualCamera::FrustumPlane* plan) {
		Eigen::Vector3f c = 0.5*(aabb->Min+aabb->Max);
		Eigen::Vector3f e = aabb->Max-c;
		
		// bounding box span in normal direction of plane
		float r = e[0]*abs(plan->n[0])
		        + e[1]*abs(plan->n[1])
		        + e[2]*abs(plan->n[2]);
		
		// distance center to plane
		float s = (plan->n.dot(c) - plan->dist);
		return s > -r;
	}

	void LODActor::setFaceCulling(bool state) {
		m_faceCulling = state;
	}

	void LODActor::clearMatRef() {
		for (uint32_t i = 0; i < m_instancedMatRef.size(); i++)
			m_instancedMatRef[i]->clear();
		if (!m_isManualInstaned)
			m_instancedMatrices.clear();
	}

	std::vector<T3DMesh<float>*> LODActor::getLODMeshes() {
		return m_LODMeshes;
	}

	void LODActor::setBufferData(void) {

		// bind array and index buffer
		if (storeOnVRAM) {
			m_pVertexBuffers[m_LODLevel]->bind();
			m_pElementBuffers[m_LODLevel]->bind();
		} else {
			m_VertexBuffer.bind();
			m_ElementBuffer.bind();
		}
		// position array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_POSITION)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)(m_VertexUtility.offset(VertexUtility::VPROP_POSITION)));
		}

		// normal array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_NORMAL)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_NORMAL));
		}

		// tangent array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_TANGENT)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_TANGENT));
		}

		// texture coordinates
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_UVW)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_UVW));
		}

		// vertex colors
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_COLOR)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_COLOR));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_COLOR), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_COLOR));
		}

		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_BONEINDICES)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_INDICES));
			glVertexAttribIPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_INDICES), 4, GL_INT, m_VertexUtility.vertexSize(), (const void*)m_VertexUtility.offset(VertexUtility::VPROP_BONEINDICES));
		}

		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_BONEWEIGHTS)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_WEIGHTS));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_WEIGHTS), 4, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (const void*)m_VertexUtility.offset(VertexUtility::VPROP_BONEWEIGHTS));
		}
	}//setBufferData
}
