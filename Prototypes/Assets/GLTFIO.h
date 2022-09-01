/*****************************************************************************\
*                                                                           *
* File(s): GLTFIO.h and GLTFIO.cpp                                            *
*                                                                           *
* Content: Import/Export class for glTF format using tinygltf   *
*                                                   *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GLTFIO_H__
#define __CFORGE_GLTFIO_H__

#include "../../CForge/AssetIO/I3DMeshIO.h"
#include <tiny_gltf.h>
#include <algorithm>


namespace CForge {
class GLTFIO {
	public:
		GLTFIO(void);
		~GLTFIO(void);

		void load(const std::string Filepath, T3DMesh<float>* pMesh);
		void store(const std::string Filepath, const T3DMesh<float>* pMesh);
		
		void release(void);
		static bool accepted(const std::string Filepath, I3DMeshIO::Operation Op);

		static int componentCount(const int type);

		static int sizeOfGltfComponentType(const int componentType);

		static bool componentIsMatrix(const int type);

		static void toVec3f(std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector3f>* pOut);

		static void toVec4f(std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector4f>* pOut);

		static void toQuatf(std::vector<std::vector<float>>* pIn, std::vector<Eigen::Quaternionf>* pOut);

		static void fromVec3f(std::vector<Eigen::Vector3f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromVec4f(std::vector<Eigen::Vector4f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromQuatf(std::vector<Eigen::Quaternionf>* pIn, std::vector<std::vector<float>>* pOut);

	protected:
		std::string filePath;

		tinygltf::Model model;
		T3DMesh<float>* pMesh;
		const T3DMesh<float>* pCMesh;
		// Data for every primitive is stored in a separate vector.
		
		std::vector<Eigen::Matrix<float, 3, 1>> coord;
		std::vector<Eigen::Matrix<float, 3, 1>> normal;
		std::vector<Eigen::Matrix<float, 3, 1>> tangent;
		std::vector<Eigen::Matrix<float, 3, 1>> texCoord;
		std::vector<Eigen::Matrix<float, 4, 1>> color;
		std::vector<Eigen::Matrix<float, 4, 1>> joint;
		std::vector<Eigen::Matrix<float, 4, 1>> weight;

		std::vector<unsigned long> offsets;
		unsigned long materialIndex;

		template<class T>
		void readBuffer(unsigned char* pBuffer, const int element_count, const int offset, const int component_count, const bool is_matrix, const int stride, std::vector<T>* pData) {
			T* raw_data = (T*)pBuffer;

			int type_size = sizeof(T);
			
			if (is_matrix) {
				//A matrix is stored in column major order in glTF.
				//Every column has to start at an index which is a multiple 4.
				//Padding bytes have to be inserted to fill the gaps.
				int row_count = std::sqrt(component_count);
				int column_size = row_count * type_size;
				int column_size_with_padding = column_size + (4 - column_size) % 4;

				for (int i = 0; i < element_count * row_count; i++) {
					int index = offset / type_size + i * (column_size_with_padding / type_size);

					for (int k = 0; k < column_size_with_padding; k++) {
						if (k >= column_size) continue;
						pData->push_back(raw_data[index + k]);
					}
				}
			}
			else {
				for (int i = 0; i < element_count; i++) {
					int index = offset / type_size + i * (component_count + stride / type_size);
					for (int k = 0; k < component_count; k++) {
						pData->push_back(raw_data[index + k]);
					}
				}
				
			}
		}

		template<class T>
		void getAccessorDataScalar(const int accessor, std::vector<T>* pData) {
			Accessor acc = model.accessors[accessor];
			BufferView buffView = model.bufferViews[acc.bufferView];
			Buffer buff = model.buffers[buffView.buffer];

			if (acc.type != TINYGLTF_TYPE_SCALAR) {
				std::cout << "Called getAccessorDataScalar on a non scalar accessor!" << std::endl;
				return;
			}

			readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, pData);
		}

		void getAccessorDataScalarFloat(const int accessor, std::vector<float>* pData);

		template<class T>
		void getAccessorData(const int accessor, std::vector<std::vector<T>>* pData) {
			Accessor acc = model.accessors[accessor];
			BufferView buffView = model.bufferViews[acc.bufferView];
			Buffer buff = model.buffers[buffView.buffer];

			if (acc.type == TINYGLTF_TYPE_SCALAR) {
				std::cout << "Called getAccessorData on a scalar accessor!" << std::endl;
				return;
			}

			int nComponents = componentCount(acc.type);

			std::vector<T> unstructured;

			readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, nComponents, componentIsMatrix(acc.type), buffView.byteStride, &unstructured);

			for (int i = 0; i < unstructured.size(); i += nComponents) {
				std::vector<T> toAdd;
				for (int j = 0; j < nComponents; j++) {
					toAdd.push_back(unstructured[i + j]);
				}
				pData->push_back(toAdd);
			}
		}

		int32_t getSparseAccessorData(const int accessor, std::vector<int32_t>* pIndices, std::vector<std::vector<float>>* pData);

		void getAccessorDataFloat(const int accessor, std::vector<std::vector<float>>* pData);
		
		void getAccessorData(const int accessor, std::vector<Eigen::Vector3f>* pData);

		void getAccessorData(const int accessor, std::vector<Eigen::Vector4f>* pData);

		void getAccessorData(const int accessor, std::vector<Eigen::Quaternionf>* pData);
		
		template<class T>
		void writeBuffer(std::vector<unsigned char>* pBuffer, const int offset, const int component_count, const bool is_matrix, const int stride, std::vector<T>* pData) {
			int type_size = sizeof(T);

			while (pBuffer->size() < offset) pBuffer->push_back(0);

			int index = offset;

			if (is_matrix) {
				int column_size = std::sqrt(component_count);
				int column_size_bytes = column_size * type_size;
				int padding = 4 - column_size_bytes % 4;

				for (int i = 0; i < pData->size(); i++) {
					T element = (*pData)[i];
					unsigned char* as_char_pointer = (unsigned char*) &element;

					for (int j = 0; j < column_size; j++) {
						for (int k = 0; k < column_size_bytes; k++) {
							if (index == pBuffer->size()) pBuffer->push_back(as_char_pointer[k]);
							else (*pBuffer)[index] = as_char_pointer[k];
							index++;
						}

						for (int k = 0; k < padding; k++) {
							if (index == pBuffer->size()) pBuffer->push_back(0);
							index++;
						}
					}
				}
			}
			else {
				for (int i = 0; i < pData->size(); i++) {
					T element = (*pData)[i];
					unsigned char* as_char_pointer = (unsigned char*) &element;

					for (int k = 0; k < type_size; k++) {
						if (index == pBuffer->size()) pBuffer->push_back(as_char_pointer[k]);
						else (*pBuffer)[index] = as_char_pointer[k];
						index++;
					}

					for (int k = 0; k < stride; k++) {
						if (index == pBuffer->size()) pBuffer->push_back(0);
						index++;
					}
				}
			}
		}

		template<class T>
		void writeAccessorDataScalar(const int bufferIndex, const int componentType, std::vector<T>* pData) {
			std::cout << "write accessor size: " << pData->size() << ", scalar" << std::endl;
			
			Buffer* pBuffer = &model.buffers[bufferIndex];

			Accessor accessor;

			accessor.bufferView = model.bufferViews.size();
			accessor.byteOffset = 0;
			accessor.componentType = componentType;
			accessor.type = TINYGLTF_TYPE_SCALAR;
			accessor.count = pData->size();

			model.accessors.push_back(accessor);

			BufferView bufferView;

			bufferView.byteOffset = pBuffer->data.size();
			bufferView.buffer = bufferIndex;
			bufferView.byteLength = pData->size() * sizeof(T);

			model.bufferViews.push_back(bufferView);

			for (int i = 0; i < pData->size(); i++) {
				for (int k = 0; k < sizeof(T); k++) {
					pBuffer->data.push_back(((unsigned char*)&(*pData)[i])[k]);
				}
			}
		}

		template<class T>
		void writeAccessorData(const int bufferIndex, const int type, std::vector<std::vector<T>>* pData) {
			std::cout << "write accessor size: " << pData->size() << ", vec " << (*pData)[0].size() << std::endl;

			Buffer* pBuffer = &model.buffers[bufferIndex];

			Accessor accessor;

			accessor.bufferView = model.bufferViews.size();
			accessor.byteOffset = 0;
			accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
			accessor.type = type;
			accessor.count = pData->size();

			model.accessors.push_back(accessor);

			BufferView bufferView;

			bufferView.byteOffset = pBuffer->data.size();
			bufferView.buffer = bufferIndex;
			bufferView.byteLength = pData->size() * sizeof(T) * componentCount(type);
			bufferView.byteStride = 0;

			model.bufferViews.push_back(bufferView);

			bool is_matrix = componentIsMatrix(type);

			std::vector<T> simplified;
			for (auto v : *pData) {
				for (auto e : v) simplified.push_back(e);
			}
			
			writeBuffer(&pBuffer->data, bufferView.byteOffset + accessor.byteOffset, accessor.count, is_matrix, bufferView.byteStride, &simplified);

			/*
			for (int i = 0; i < pData->size(); i++) {
				for (int j = 0; j < (*pData)[i].size(); j++) {
					for (int k = 0; k < sizeof(T); k++) {
						pBuffer->data.push_back(((unsigned char*)&((*pData)[i][j]))[k]);
					}
				}
			}
			*/
		}

		void writeSparseAccessorData(const int buffer_index, const int type, std::vector<int32_t>* pIndices, std::vector<std::vector<float>>* pData);

		void readMeshes();

		void readPrimitive(tinygltf::Primitive* pPrimitive);

		void readAttributes(tinygltf::Primitive* pPrimitive);

		void readSubMeshes(tinygltf::Primitive* pPrimitive);
		
		void readIndices(const int accessorIndex, std::vector<int>* pIndices);

		void readFaces(tinygltf::Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces);

		void readMaterial(const int materialIndex, T3DMesh<float>::Material* pMaterial);

		void readNodes();

		std::string getTexturePath(const int textureIndex);

		void readSkeletalAnimations();

		void readSkinningData();

		void readMorphTargets();

		
		void writeMeshes();

		void writePrimitive();

		void writeAttributes();

		void prepareAttributeArrays();

		void writeMaterial();

		int writeTexture(const std::string path);

		void writeNodes();
	};//GLTFIO

}//name space

#endif 