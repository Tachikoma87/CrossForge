#include "InstanceActor.h"

using namespace Eigen;

namespace CForge {
	InstanceActor::InstanceActor(void) : IRenderableActor("StaticActor", ATYPE_STATIC) {
		m_TypeID = ATYPE_STATIC;
		m_TypeName = "Static Actor";
	}//Constructor

	InstanceActor::~InstanceActor(void) {
		clear();
	}//Destructor

	void InstanceActor::init(const T3DMesh<float>* pMesh) {

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");

		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= InstanceVertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= InstanceVertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= InstanceVertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= InstanceVertexUtility::VPROP_UVW;

		// build array buffer of vertex data
		void* pBuffer = nullptr;
		uint32_t BufferSize = 0;

		try {
			m_InstanceVertexUtility.init(VertexProperties);
			m_InstanceVertexUtility.buildBuffer(pMesh->vertexCount(), &pBuffer, &BufferSize, pMesh, transMatrixes);

			instanceAmmount = transMatrixes.size();
			instanceOffset = BufferSize - instanceAmmount * sizeof(float) * 16;
			
			m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
			// free buffer data
			delete[] pBuffer;
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

		if (firstInit) {
			firstInit = false;
			
			// build render groups and element array
			try {
				m_RenderGroupUtility.init(pMesh, &pBuffer, &BufferSize);
				m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
				// free buffer data
				delete[] pBuffer;
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
		}
		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();

	}//initialize

	void InstanceActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_VertexArray.clear();

		m_InstanceVertexUtility.clear();
		m_RenderGroupUtility.clear();
	}//Clear

	void InstanceActor::release(void) {
		delete this;
	}//release

	uint32_t InstanceActor::materialCount(void) const {
		return m_RenderGroupUtility.renderGroupCount();
	}//materialCount

	RenderMaterial* InstanceActor::material(uint32_t Index) {
		if (Index >= m_RenderGroupUtility.renderGroupCount()) throw IndexOutOfBoundsExcept("Index");
		return &(m_RenderGroupUtility.renderGroups()[Index]->Material);
	}//material

	void InstanceActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		m_VertexArray.bind();

		for (auto i : m_RenderGroupUtility.renderGroups()) {

			/*if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				pRDev->activeShader(pRDev->shadowPassShader());
			}
			else {
				pRDev->activeShader(i->pShader);
				pRDev->activeMaterial(&i->Material);
			}*/

			glDrawElementsInstanced(GL_TRIANGLES, m_ElementBuffer.size(), GL_UNSIGNED_INT, 0, instanceAmmount);
		}//for[all render groups]

	}//render

	void InstanceActor::setBufferData(void) {
		// bind array and index buffer
		m_VertexBuffer.bind();
		m_ElementBuffer.bind();

		// position array
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_InstanceVertexUtility.vertexSize(), (void*)(m_InstanceVertexUtility.offset(InstanceVertexUtility::VPROP_POSITION)));
	
		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL), 3, GL_FLOAT, GL_FALSE, m_InstanceVertexUtility.vertexSize(), (void*)m_InstanceVertexUtility.offset(InstanceVertexUtility::VPROP_NORMAL));
		
		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT), 3, GL_FLOAT, GL_FALSE, m_InstanceVertexUtility.vertexSize(), (void*)m_InstanceVertexUtility.offset(InstanceVertexUtility::VPROP_TANGENT));
		
		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 3, GL_FLOAT, GL_FALSE, m_InstanceVertexUtility.vertexSize(), (void*)m_InstanceVertexUtility.offset(InstanceVertexUtility::VPROP_UVW));
		
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)instanceOffset);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(1 * sizeof(float) * 4 + instanceOffset));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(2 * sizeof(float) * 4 + instanceOffset));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(3 * sizeof(float) * 4 + instanceOffset));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

	}//setBufferData

	void InstanceActor::addInstance(Matrix4f matrix) {
		transMatrixes.push_back(matrix);
	}

	void InstanceActor::clearInstances() {
		transMatrixes.clear();
	}
}