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

#pragma region util
		static int componentCount(const int type);

		static int sizeOfGltfComponentType(const int componentType);

		static bool componentIsMatrix(const int type);

		static void toVec2f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector2f>* pOut);

		static void toVec3f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector3f>* pOut);

		static void toVec4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Vector4f>* pOut);

		static void toQuatf(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Quaternionf>* pOut);

		static void toMat4f(const std::vector<std::vector<float>>* pIn, std::vector<Eigen::Matrix4f>* pOut);

		static Eigen::Matrix4f toSingleMat4(const std::vector<double> *pin);

		static void fromVec2f(const std::vector<Eigen::Vector2f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromVec3f(const std::vector<Eigen::Vector3f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromVec4f(const std::vector<Eigen::Vector4f>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromQuatf(const std::vector<Eigen::Quaternionf>* pIn, std::vector<std::vector<float>>* pOut);

		static void fromMat4f(const std::vector<Eigen::Matrix4f>*  pIn, std::vector<std::vector<float>>* pOut);

		static void fromUVtoUVW(const std::vector<Eigen::Vector2f>* pIn, std::vector<Eigen::Vector3f>* pOut);
		
		static void fromUVWtoUV(const std::vector<Eigen::Vector3f>* pIn, std::vector<Eigen::Vector2f>* pOut);

		int getMeshIndexByCrossForgeVertexIndex(int index);

		int getGltfComponentType(const float value) { return TINYGLTF_COMPONENT_TYPE_FLOAT; }

		int getGltfComponentType(const unsigned char value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE; }

		int getGltfComponentType(const char value) { return TINYGLTF_COMPONENT_TYPE_BYTE; }

		int getGltfComponentType(const unsigned short value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT; }

		int getGltfComponentType(const short value) { return TINYGLTF_COMPONENT_TYPE_SHORT; }

		int getGltfComponentType(const unsigned int value) { return TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT; }
		
		int getGltfComponentType(const int value) { return TINYGLTF_COMPONENT_TYPE_INT; }

		Eigen::Vector3f getTranslation(const Eigen::Matrix4f& transformation);

		Eigen::Quaternionf getRotation(const Eigen::Matrix4f& transformation);

		Eigen::Vector3f getScale(const Eigen::Matrix4f& transformation);

#pragma endregion
	protected:
		std::string filePath;

		tinygltf::Model model;
		T3DMesh<float>* pMesh;
		const T3DMesh<float>* pCMesh;
		// Data for every primitive is stored in a separate vector.
		
		std::vector<Eigen::Matrix<float, 3, 1>> coord;
		std::vector<Eigen::Matrix<float, 3, 1>> normal;
		std::vector<Eigen::Matrix<float, 3, 1>> tangent;
		std::vector<Eigen::Matrix<float, 2, 1>> texCoord;
		std::vector<Eigen::Matrix<float, 4, 1>> color;
		std::vector<Eigen::Matrix<float, 4, 1>> joint;
		std::vector<Eigen::Matrix<float, 4, 1>> weight;

		std::vector<std::pair<int32_t, int32_t>> primitiveIndexRanges;

		std::vector<unsigned long> offsets;
		unsigned long materialIndex;

#pragma region accessor_read
		template<class T>
		void readBuffer(unsigned char* pBuffer, const int element_count, const int offset, const int component_count, const bool is_matrix, const int stride, std::vector<T>* pData) {
			pData->reserve(element_count * component_count);

			T* raw_data = (T*)(pBuffer + offset);

			int type_size = sizeof(T);
			
			if (is_matrix) {
				//A matrix is stored in column major order in glTF.
				//Every column has to start at an index which is a multiple 4.
				//Padding bytes have to be inserted to fill the gaps.
				int row_count = std::sqrt(component_count);
				int column_size = row_count * type_size;
				int column_size_with_padding = column_size + (4 - column_size) % 4;

				for (int i = 0; i < element_count * row_count; i++) {
					int index = i * (column_size_with_padding / type_size);

					for (int k = 0; k < column_size_with_padding; k++) {
						if (k >= column_size) continue;
						pData->push_back(raw_data[index + k]);
					}
				}
			}
			else {
				int jump = stride / type_size;
				if (jump == 0) jump = component_count;
				
				for (int i = 0; i < element_count; i++) {
					int index = i * jump;
					for (int k = 0; k < component_count; k++) {
						T tmp = raw_data[index + k];
						pData->push_back(tmp);
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

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
				std::vector<char> data;
				
				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
				std::vector<unsigned char> data;

				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
				std::vector<short> data;

				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
				std::vector<unsigned short> data;
				
				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_INT) {
				std::vector<int32_t> data;
				
				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
				std::vector<int32_t> data;

				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}

			if (acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
				std::vector<float> data;
				
				readBuffer(buff.data.data(), acc.count, acc.byteOffset + buffView.byteOffset, 1, false, 0, &data);
				for (auto d : data) pData->push_back((T)d);
				return;
			}
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

			int remaining_size = buff.data.size() - acc.byteOffset - buffView.byteOffset;

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
#pragma endregion
		
#pragma region accessor_write
		template<class T>
		void writeBuffer(std::vector<unsigned char>* pBuffer, const int offset, const int component_count, const bool is_matrix, const int stride, const std::vector<T>* pData) {
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
		void writeAccessorDataScalar(const int bufferIndex, std::vector<T>* pData) {
			std::cout << "write accessor size: " << pData->size() << ", scalar" << std::endl;
			
			Buffer* pBuffer = &model.buffers[bufferIndex];

			Accessor accessor;

			T min = std::numeric_limits<T>::max();
			T max = std::numeric_limits<T>::lowest();

			for (int i = 0; i < pData->size(); i++) {
				T element = (*pData)[i];
				if (element < min) min = element;
				if (element > max) max = element;
			}

			accessor.minValues.push_back(min);
			accessor.maxValues.push_back(max);

			T t = 0;

			accessor.bufferView = model.bufferViews.size();
			accessor.byteOffset = 0;
			accessor.componentType = getGltfComponentType(t);
			accessor.type = TINYGLTF_TYPE_SCALAR;
			accessor.count = pData->size();

			model.accessors.push_back(accessor);

			BufferView bufferView;

			bufferView.byteOffset = pBuffer->data.size();
			bufferView.buffer = bufferIndex;
			bufferView.byteLength = pData->size() * sizeof(T);

			bufferView.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

			model.bufferViews.push_back(bufferView);

			writeBuffer(&pBuffer->data, bufferView.byteOffset + accessor.byteOffset, 
				accessor.count, false, bufferView.byteStride, pData);
		}

		template<class T>
		void writeAccessorData(const int bufferIndex, const int type, std::vector<std::vector<T>>* pData) {
			std::cout << "write accessor size: " << pData->size() << ", vec " << (*pData)[0].size() << std::endl;

			Buffer* pBuffer = &model.buffers[bufferIndex];

			Accessor accessor;
			
			T n_min = std::numeric_limits<T>::max();
			T n_max = std::numeric_limits<T>::lowest();

			std::vector<T> min;
			std::vector<T> max;

			for (int i = 0; i < (*pData)[0].size(); i++) {
				min.push_back(n_min);
				max.push_back(n_max);
			}

			for (int i = 0; i < pData->size(); i++) {
				auto element = (*pData)[i];

				for (int j = 0; j < element.size(); j++) {
					T value = element[j];
					if (value < min[j]) min[j] = value;
					if (value > max[j]) max[j] = value;
				}
			}

			for (int i = 0; i < min.size(); i++) {
				accessor.minValues.push_back(min[i]);
				accessor.maxValues.push_back(max[i]);
			}
			
			T t = 0;
			
			accessor.bufferView = model.bufferViews.size();
			accessor.byteOffset = 0;
			accessor.componentType = getGltfComponentType(t);
			accessor.type = type;
			accessor.count = pData->size();

			model.accessors.push_back(accessor);

			BufferView bufferView;

			bufferView.byteOffset = pBuffer->data.size();
			bufferView.buffer = bufferIndex;
			bufferView.byteLength = pData->size() * sizeof(T) * componentCount(type);
			bufferView.byteStride = 0;

			bufferView.target = TINYGLTF_TARGET_ARRAY_BUFFER;

			model.bufferViews.push_back(bufferView);

			bool is_matrix = componentIsMatrix(type);

			std::vector<T> simplified;
			for (auto v : *pData) {
				for (auto e : v) simplified.push_back(e);
			}
			
			writeBuffer(&pBuffer->data, bufferView.byteOffset + accessor.byteOffset, accessor.count, is_matrix, bufferView.byteStride, &simplified);
		}

		int writeSparseAccessorData(const int buffer_index, const int type, const std::vector<int32_t>* pIndices, const std::vector<std::vector<float>>* pData);
#pragma endregion

#pragma region read
		void readMeshes();

		T3DMesh<float>::Submesh* readPrimitive(tinygltf::Primitive* pPrimitive);

		void readAttributes(tinygltf::Primitive* pPrimitive);

		T3DMesh<float>::Submesh* readSubMeshes(tinygltf::Primitive* pPrimitive);

		void readFaces(tinygltf::Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces);

		void readMaterial(const int materialIndex, T3DMesh<float>::Material* pMaterial);

		void readNodes();

		std::string getTexturePath(const int textureIndex);

		void readSkeletalAnimations();

		void readSkinningData();

		void readMorphTargets();
#pragma endregion

#pragma region write
		int writePrimitive(const T3DMesh<float>::Submesh* pSubmesh);

		void writeAttributes();

		std::pair<int, int> prepareAttributeArrays(const T3DMesh<float>::Submesh* pSubmesh);

		void writeMaterial(const T3DMesh<float>::Submesh* pSubmesh);

		int writeTexture(const std::string path);

		void writeNodes();

		void writeMorphTargets(std::pair<int, int> minmax);

		void writeSkinningData();
#pragma endregion
	};//GLTFIO

}//name space

#endif 