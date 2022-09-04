#include <glad/glad.h>
#include "IRenderableActor.h"
#include "../RenderDevice.h"

namespace CForge {

	IRenderableActor::IRenderableActor(const std::string ClassName, int32_t ActorType): CForgeObject("IRenderableActor::" + ClassName) {
		m_TypeID = ActorType;
	}//Constructor

	IRenderableActor::~IRenderableActor(void) {

	}//Destructor

	int32_t IRenderableActor::typeID(void)const {
		return m_TypeID;
	}//typeID


	void IRenderableActor::setBufferData(void) {

		// bind array and index buffer
		m_VertexBuffer.bind();
		m_ElementBuffer.bind();

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

	uint32_t IRenderableActor::materialCount(void) const {
		return m_RenderGroupUtility.renderGroupCount();
	}//materialCount

	RenderMaterial* IRenderableActor::material(uint32_t Index) {
		if (Index >= m_RenderGroupUtility.renderGroupCount()) throw IndexOutOfBoundsExcept("Index");
		return &(m_RenderGroupUtility.renderGroups()[Index]->Material);
	}//material

	T3DMesh<float>::AABB IRenderableActor::getAABB()
	{
		T3DMesh<float>::AABB aabb;
		aabb.Min = Eigen::Vector3f(0);
		aabb.Max = Eigen::Vector3f(0);
		return aabb;
	}
	
	void IRenderableActor::testAABBvis(RenderDevice* pRDev, Eigen::Matrix4f sgMat)
	{
	
	}

	bool IRenderableActor::isInstanced()
	{
		return m_isInstanced;
	}

	bool IRenderableActor::isManualInstanced()
	{
		return m_isManualInstaned;
	}

	bool IRenderableActor::isInLODSG() {
		return m_isInLODSG;
	}
	void IRenderableActor::setLODSG(bool inside) {
		m_isInLODSG = inside;
	}

	void IRenderableActor::bindLODLevel(uint32_t level) {

	}

	void IRenderableActor::addInstance(Eigen::Matrix4f matrix) {

	}

	void IRenderableActor::evaluateQueryResult(Eigen::Matrix4f mat, GLint pixelCount) {
	
	}

	void IRenderableActor::clearMatRef() {

	}

	float IRenderableActor::getAABBradius(const Eigen::Matrix4f& mat) {
		Eigen::Affine3f affine(mat);
		affine.data()[12] = 0.0;
		affine.data()[13] = 0.0;
		affine.data()[14] = 0.0;
		Eigen::Vector3f scaledAABBMax = affine * getAABB().Max;
		Eigen::Vector3f scaledAABBMin = affine * getAABB().Min;
		Eigen::Vector3f center = scaledAABBMin*0.5+scaledAABBMax*0.5;
		scaledAABBMax -= center;
		scaledAABBMin -= center;

		float aabbRadius = std::max(std::abs(scaledAABBMax.norm()), std::abs(scaledAABBMin.norm()));
		return aabbRadius;
	}

	Eigen::Vector3f IRenderableActor::getAABBcenter(const Eigen::Matrix4f& mat) {
		Eigen::Affine3f affine(mat);
		affine.data()[12] = 0.0;
		affine.data()[13] = 0.0;
		affine.data()[14] = 0.0;
		Eigen::Vector3f scaledAABBMax = affine * getAABB().Max;
		Eigen::Vector3f scaledAABBMin = affine * getAABB().Min;

		Eigen::Vector3f center = scaledAABBMin*0.5+scaledAABBMax*0.5;
		
		return center;
	}
}//name space