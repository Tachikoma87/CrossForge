#include "UBOCameraData.h"
using namespace Eigen;

namespace CForge {

	UBOCameraData::UBOCameraData(void): CForgeObject("UBOCameraData") {
		m_ProjectionMatrixOffset = 0;
		m_ViewMatrixOffset = 0;
		m_Positionoffset = 0;
	}//Constructor

	UBOCameraData::~UBOCameraData(void) {
		clear();
	}//Destructor

	void UBOCameraData::init(void) {
		clear();

		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());
		m_ViewMatrixOffset = 0;
		m_ProjectionMatrixOffset = 16 * sizeof(float);
		m_Positionoffset = 2 * 16 * sizeof(float);

	}//initialize

	void UBOCameraData::clear(void) {

	}//clear

	void UBOCameraData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	void UBOCameraData::viewMatrix(const Eigen::Matrix4f Mat) {
		m_Buffer.bufferSubData(m_ViewMatrixOffset, 16 * sizeof(float), Mat.data());
	}//viewMatrix

	void UBOCameraData::projectionMatrix(const Eigen::Matrix4f Mat) {
		m_Buffer.bufferSubData(m_ProjectionMatrixOffset, 16 * sizeof(float), Mat.data());
	}//projectionMatrix

	void UBOCameraData::position(const Eigen::Vector3f Pos) {
		m_Buffer.bufferSubData(m_Positionoffset, 3 * sizeof(float), Pos.data());
	}//position

	uint32_t UBOCameraData::size(void)const {
		uint32_t Rval = 0;
		Rval += 16 * sizeof(float); // View Matrix
		Rval += 16 * sizeof(float); // projection matrix
		Rval += 4 * sizeof(float);
		return Rval;
	}//size

}//name space