/*****************************************************************************\
*                                                                           *
* File(s): T3DHair.hpp                                                         *
*                                                                           *
* Content:                                                                  *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann, Sascha Jüngel                                     *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_T3DHAIR_H__
#define __CFORGE_T3DHAIR_H__

#include "../../CForge/Core/CForgeObject.h"
#include "../../CForge/Core/CoreUtility.hpp"

namespace CForge {

	/**
	* \brief Template class that stores hair strand data.
	*
	* \todo Do full documentation
	*/

	template<typename T>
	class T3DHair: public CForgeObject {
	public:

		struct Strand {
			int32_t* Vertices;
			int32_t* Tangents;
		};//strand

		struct Material {
			int32_t ID;
			Eigen::Vector4f Color;

			std::vector<std::string> VertexShaderSources;
			std::vector<std::string> FragmentShaderSources;

			Material(void) {
				ID = -1;
				Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

				VertexShaderSources.push_back("../Shader/HairKajiya.vert");
				FragmentShaderSources.push_back("../Shader/HairKajiya.frag");
			}//constructor

			~Material(void) {
				clear();
			}//destructor

			void init(const Material* pMat = nullptr) {
				clear();
				if (nullptr != pMat) {
					ID = pMat->ID;
					Color = pMat->Color;

					VertexShaderSources = pMat->VertexShaderSources;
					FragmentShaderSources = pMat->FragmentShaderSources;
				}
			}//initialize

			void clear(void) {
				ID = -1;
				Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				VertexShaderSources.clear();
				FragmentShaderSources.clear();
			}//clear

		};//material

		struct AABB {
			Eigen::Vector3f Min;
			Eigen::Vector3f Max;

			Eigen::Vector3f diagonal(void) {
				return (Max - Min);
			}
		};//AxisAlignedBoundingBox

		T3DHair(void): CForgeObject("T3DHair") {
			m_Material = new Material;
		}//constructor

		~T3DHair(void) {
			clear();
		}//destructor

		void init(const T3DHair* pRef = nullptr) {
			if (nullptr != pRef && pRef != this) {
				clear();
				m_Positions = pRef->m_Positions;
				m_Tangents = pRef->m_Tangents;
				m_Material = pRef->m_Material;
				m_AABB = pRef->m_AABB;
			}
		}//initialize

		void clear(void) {
			m_Positions.clear();
			m_Tangents.clear();
			m_Material->clear();
		}//clear

		////// Setter
		void vertices(std::vector<Eigen::Matrix<T, 3, 1>> *pCoords) {
			if (nullptr != pCoords) m_Positions = (*pCoords);
		}//positions

		void tangents(std::vector<Eigen::Matrix<T, 3, 1>>* pTangents) {
			if (nullptr != pTangents) m_Tangents = (*pTangents);
		}//tangents

		void color(float r, float g, float b, float a) {
			m_Material->Color = Eigen::Vector4f(r,g,b,a);
		}//color

		void setMaterial(Material* pMat) {
			m_Material->init(pMat);
		}//setMaterial

		void strandCount(uint32_t value) {
			m_numStrands = value;
		}//strandCount

		void start(int* value) {
			m_start = value;
		}
		void count(int* value) {
			m_count = value;
		}

		///
		uint32_t vertexCount(void)const {
			return m_Positions.size();
		}//vertexCount

		uint32_t strandCount(void)const {
			return m_numStrands;
		}//strandCount

		////////// Getter
		Eigen::Matrix<T, 3, 1>& vertex(int32_t Index) {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		const Eigen::Matrix<T, 3, 1> vertex(int32_t Index) const {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		Eigen::Matrix<T, 3, 1>& tangent(int32_t Index) {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Tangents[Index];
		}//tangent

		const Eigen::Matrix<T, 3, 1> tangent(int32_t Index) const {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Tangents[Index];
		}//tangent

		Eigen::Vector4f color() {
			return m_Material->Color;
		}//color

		const Eigen::Vector4f color() const {
			return m_Material->Color;
		}//color

		Material* getMaterial() {
			return m_Material;
		}//getMaterial

		const Material* getMaterial() const {
			return m_Material;
		}//getMaterial

		AABB aabb(void)const {
			return m_AABB;
		}//aabb

		int* start(void)const {
			return m_start;
		}
		int* count(void)const {
			return m_count;
		}

		void computeAxisAlignedBoundingBox(void) {
			if (m_Positions.size() == 0) throw CForgeExcept("Hair has no vertex data. Can not compute axis aligned bounding box");
			m_AABB.Min = m_Positions[0];
			m_AABB.Max = m_Positions[0];
			for (auto i : m_Positions) {
				if (i.x() < m_AABB.Min.x()) m_AABB.Min.x() = i.x();
				if (i.y() < m_AABB.Min.y()) m_AABB.Min.y() = i.y();
				if (i.z() < m_AABB.Min.z()) m_AABB.Min.z() = i.z();
				if (i.x() > m_AABB.Max.x()) m_AABB.Max.x() = i.x();
				if (i.y() > m_AABB.Max.y()) m_AABB.Max.y() = i.y();
				if (i.z() > m_AABB.Max.z()) m_AABB.Max.z() = i.z();
			}//for[all position values]
		}//computeAxisAlignedBoundingBox

	protected:
		std::vector<Eigen::Matrix<T, 3, 1>> m_Positions; // Vertex positions
		std::vector<Eigen::Matrix<T, 3, 1>> m_Tangents; // per vertex tangents
		Material* m_Material; ///< Material, currently hair can only have 1
		Strand* m_Strands; ///< Hair strands
		AABB m_AABB;

		uint32_t m_numStrands;

		// [TODO] rename these
		int* m_start;
		int* m_count;
	};//T3DHair

}//name space

#endif