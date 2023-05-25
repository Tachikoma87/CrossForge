#include "GLTFIO.hpp"

#define TINYGLTF_IMPLEMENTATION
#ifndef __EMSCRIPTEN__
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#if (defined(__EMSCRIPTEN__) || defined(_MSC_VER) || (defined(__GNUC__) && (__GNUC__ > 7)) )
#include <filesystem>
#define STD_FS std::filesystem
#else
#define EXPERIMENTAL_FILESYSTEM 1
#include <experimental/filesystem>
#define STD_FS std::experimental::filesystem
#endif

#include <iostream>
#include <algorithm>
#include <fstream>

using namespace tinygltf;

namespace CForge {

#pragma region read

	void GLTFIO::readMeshes() {
		for (int i = 0; i < m_model.meshes.size(); i++) {
			Mesh currentMesh = m_model.meshes[i];

			T3DMesh<float>::Submesh* pParentSubmesh;

			for (int j = 0; j < m_model.nodes.size(); j++) {
				if (m_model.nodes[j].mesh == i) {
					pParentSubmesh = m_pMesh->getSubmesh(j);
					break;
				}
			}

			for (int k = 0; k < currentMesh.primitives.size(); k++) {
				Primitive currentPrimitive = currentMesh.primitives[k];

				auto pSubmesh = readPrimitive(&currentPrimitive);

				pParentSubmesh->Children.push_back(pSubmesh);
				pSubmesh->pParent = pParentSubmesh;
			}
		}

		auto pCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_coord) pCoord->push_back(i);
		auto pNormal = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_normal) pNormal->push_back(i);
		auto pTangent = new std::vector<Eigen::Matrix<float, 3, 1>>;
		for (auto i : m_tangent) pTangent->push_back(i);
		auto pTexCoord = new std::vector<Eigen::Matrix<float, 3, 1>>;
		fromUVtoUVW(&m_texCoord, pTexCoord);

		m_pMesh->vertices(pCoord);
		m_pMesh->normals(pNormal);
		m_pMesh->tangents(pTangent);
		m_pMesh->textureCoordinates(pTexCoord);
	}

	T3DMesh<float>::Submesh* GLTFIO::readPrimitive(Primitive* pPrimitive) {
		readAttributes(pPrimitive);

		return readSubMeshes(pPrimitive);
	}

	void GLTFIO::readAttributes(Primitive* pPrimitive) {
		int counter = 0;

		for (auto keyValuePair : pPrimitive->attributes) {
			if (keyValuePair.first == "POSITION") {
				std::vector<std::vector<float>> positions;
				getAccessorData(keyValuePair.second, &positions);

				for (auto pos : positions) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					m_coord.push_back(mat);

					counter++;
				}

				continue;
			}

			if (keyValuePair.first == "NORMAL") {
				std::vector<std::vector<float>> normals;
				getAccessorData(keyValuePair.second, &normals);

				for (auto pos : normals) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					m_normal.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first == "TANGENT") {
				std::vector<std::vector<float>> tangents;
				getAccessorData(keyValuePair.second, &tangents);

				for (auto pos : tangents) {
					Eigen::Matrix<float, 3, 1> mat;

					mat(0) = pos[0];
					mat(1) = pos[1];
					mat(2) = pos[2];

					m_tangent.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("TEXCOORD") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(9);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple textures
				if (n != 0) continue;

				std::vector<std::vector<float>> texcoords;
				getAccessorData(keyValuePair.second, &texcoords);

				for (auto pos : texcoords) {
					Eigen::Matrix<float, 2, 1> mat;

					mat(0) = pos[0];
					mat(1) = 1.0f - pos[1];

					m_texCoord.push_back(mat);
				}

				continue;
			}

			if (keyValuePair.first.find("JOINTS") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(7);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple joints
				if (n != 0) continue;

				std::vector<Eigen::Vector4f> accJoints;
				getAccessorData(keyValuePair.second, &accJoints);

				for (auto j : accJoints) {
					m_joint.push_back(j);
				}

				continue;
			}

			if (keyValuePair.first.find("WEIGHTS") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple weights
				if (n != 0) continue;

				std::vector<Eigen::Vector4f> accWeights;
				getAccessorData(keyValuePair.second, &accWeights);

				for (auto w : accWeights) {
					m_weight.push_back(w);
				}

				continue;
			}

			if (keyValuePair.first.find("COLOR") != std::string::npos) {
				int n = -1;

				auto s = keyValuePair.first.substr(8);

				sscanf(s.c_str(), "%d", &n);

				//TODO: support multiple colors
				if (n != 0) continue;

				std::vector<Eigen::Vector4f> accColors;
				getAccessorData(keyValuePair.second, &accColors);

				for (auto c : accColors) {
					m_color.push_back(c);
				}

				continue;
			}
		}//for attributes

		//fill up the rest of the attributes with default values
		for (int i = m_normal.size(); i < m_coord.size(); i++) {
			Eigen::Vector3f vec(0, 0, 0);
			m_normal.push_back(vec);
		}
		for (int i = m_tangent.size(); i < m_coord.size(); i++) {
			Eigen::Vector3f vec(0, 0, 0);
			m_tangent.push_back(vec);
		}
		for (int i = m_texCoord.size(); i < m_coord.size(); i++) {
			Eigen::Vector2f vec(0, 0);
			m_texCoord.push_back(vec);
		}
		for (int i = m_joint.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(-1, -1, -1, -1);
			m_joint.push_back(vec);
		}
		for (int i = m_weight.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(0, 0, 0, 0);
			m_weight.push_back(vec);
		}
		for (int i = m_color.size(); i < m_coord.size(); i++) {
			Eigen::Vector4f vec(0, 0, 0, 0);
			m_color.push_back(vec);
		}

		//set vertex influences and weights for bones
		for (int i = 0; i < m_joint.size(); i++) {
			for (int j = 0; j < 4; j++) {
				int32_t index = (int32_t)m_joint[i](j);
				if (index < 0) continue;
				auto pBone = m_pMesh->getBone(index);
				pBone->VertexInfluences.push_back(i);
				pBone->VertexWeights.push_back(m_weight[i](j));
			}
		}

		m_offsets.push_back(counter);
	}

	T3DMesh<float>::Submesh* GLTFIO::readSubMeshes(Primitive* pPrimitive) {
		T3DMesh<float>::Submesh* pSubMesh = new T3DMesh<float>::Submesh;

		std::vector<T3DMesh<float>::Face> faces;
		readFaces(pPrimitive, &faces);
		pSubMesh->Faces = faces;

		T3DMesh<float>::Material* pMaterial = new T3DMesh<float>::Material;

		if (pPrimitive->material != -1) {
			readMaterial(pPrimitive->material, pMaterial);
		}

		m_pMesh->addMaterial(pMaterial, false);
		pSubMesh->Material = m_materialIndex;
		m_materialIndex++;

		m_pMesh->addSubmesh(pSubMesh, false);

		return pSubMesh;
	}

	void GLTFIO::readFaces(Primitive* pPrimitive, std::vector<T3DMesh<float>::Face>* faces) {
		int accessorIndex = pPrimitive->indices;

		std::vector<int> indices;
		getAccessorDataScalar(accessorIndex, &indices);

		int offset = 0;
		for (int i = 0; i < m_offsets.size() - 1; i++) offset += m_offsets[i];

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLES) {
			for (int i = 0; i < indices.size(); i += 3) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i]);
				face.Vertices[1] = (int32_t)(offset + indices[i + 1]);
				face.Vertices[2] = (int32_t)(offset + indices[i + 2]);

				faces->push_back(face);
			}

			return;
		}

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
			for (int i = 0; i < indices.size(); i++) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i]);
				face.Vertices[1] = (int32_t)(offset + indices[i + (1 + i % 2)]);
				face.Vertices[2] = (int32_t)(offset + indices[i + (2 - i % 2)]);

				faces->push_back(face);
			}

			return;
		}

		if (pPrimitive->mode == TINYGLTF_MODE_TRIANGLE_FAN) {
			for (int i = 0; i < indices.size(); i += 3) {
				T3DMesh<float>::Face face;

				face.Vertices[0] = (int32_t)(offset + indices[i + 1]);
				face.Vertices[1] = (int32_t)(offset + indices[i + 2]);
				face.Vertices[2] = (int32_t)(offset + indices[0]);

				faces->push_back(face);
			}

			return;
		}
	}

	void GLTFIO::readMaterial(const int m_materialIndex, T3DMesh<float>::Material* pMaterial) {
		Material gltfMaterial = m_model.materials[m_materialIndex];

		// pMaterial->ID = m_materialIndex;

		pMaterial->Metallic = gltfMaterial.pbrMetallicRoughness.metallicFactor;
		pMaterial->Roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;


		pMaterial->Color[0] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[0];
		pMaterial->Color[1] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[1];
		pMaterial->Color[2] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[2];
		pMaterial->Color[3] = gltfMaterial.pbrMetallicRoughness.baseColorFactor[3];


		pMaterial->TexAlbedo = getTexturePath(gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
		pMaterial->TexNormal = getTexturePath(gltfMaterial.normalTexture.index);
		pMaterial->TexMetallicRoughness = getTexturePath(gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index);
		pMaterial->TexOcclusion = getTexturePath(gltfMaterial.occlusionTexture.index);
		pMaterial->TexEmissive = getTexturePath(gltfMaterial.emissiveTexture.index);
	}

	std::string GLTFIO::getTexturePath(const int textureIndex) {
		if (textureIndex < 0 || textureIndex >= m_model.textures.size()) return "";

		int sourceIndex = m_model.textures[textureIndex].source;

		Image* pImage = &m_model.images[sourceIndex];

		STD_FS::path path = m_filePath;
		path = path.parent_path();

		if (pImage->uri.size()) {
			//combine texture uri with gltf file parent directory to get the full path
			path.append(m_model.images[sourceIndex].uri);

			return path.string();
		}

		//embedded texture

		if (pImage->mimeType == "image/jpeg") {
			path.append("image_" + std::to_string(textureIndex) + ".jpg");
		}
		else {
			path.append("image_" + std::to_string(textureIndex) + "." + pImage->mimeType.substr(6));
		}

		//no buffer view defined or buffer view is out of range
		if (pImage->bufferView < 0 || pImage->bufferView >= m_model.bufferViews.size()) return "";

		BufferView* pBufferView = &m_model.bufferViews[pImage->bufferView];
		Buffer* pBuffer = &m_model.buffers[pBufferView->buffer];

		std::ofstream fout(path.string(), std::ios::out | std::ios::binary);
		fout.write((const char*)pBuffer->data.data() + pBufferView->byteOffset, pBufferView->byteLength);
		fout.close();

		return path.string();
	}

	void GLTFIO::readSkeletalAnimations() {
		T3DMesh<float>::SkeletalAnimation* pAnim;

		int id_counter = 0;

		for (Animation animation : m_model.animations) {
			std::vector<T3DMesh<float>::BoneKeyframes*> keyframes;
			std::vector<int> inputAccessors;

			float duration = -1;

			bool pure_morph_target_animation = true;

			for (AnimationChannel channel : animation.channels) {
				T3DMesh<float>::BoneKeyframes* pBoneKF = nullptr;
				int input = animation.samplers[channel.sampler].input;

				//A new BoneKeyFrames object is needed for every BoneID and Timestamps vector.

				for (int i = 0; i < keyframes.size(); i++) {
					if (keyframes[i]->BoneID == channel.target_node && inputAccessors[i] == input) {
						pBoneKF = keyframes[i];
						break;
					}
				}

				if (pBoneKF == nullptr) {
					pBoneKF = new T3DMesh<float>::BoneKeyframes;
					pBoneKF->BoneID = channel.target_node;
					pBoneKF->ID = id_counter++;
					keyframes.push_back(pBoneKF);
					inputAccessors.push_back(input);
				}

				std::vector<float> timeValues;
				getAccessorDataScalarFloat(input, &timeValues);
				pBoneKF->Timestamps = timeValues;

				for (auto t : timeValues) duration = std::max(duration, t);

				if (channel.target_path == "translation") {
					std::vector<Eigen::Vector3f> translations;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &translations);

					pBoneKF->Positions = translations;

					pure_morph_target_animation = false;
				}
				else if (channel.target_path == "rotation") {
					std::vector<Eigen::Quaternionf> rotations;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &rotations);

					pBoneKF->Rotations = rotations;

					pure_morph_target_animation = false;
				}
				else if (channel.target_path == "scale") {
					std::vector<Eigen::Vector3f> scalings;
					int output = animation.samplers[channel.sampler].output;

					getAccessorData(output, &scalings);

					pBoneKF->Scalings = scalings;

					pure_morph_target_animation = false;
				}
				/*
				"weights" as animation channel target is ignored,
				because cross forge only stores the morph targets without animated weights.
				*/
			}

			if (pure_morph_target_animation) continue;

			pAnim = new T3DMesh<float>::SkeletalAnimation;

			pAnim->Name = animation.name;
			pAnim->Keyframes = keyframes;
			pAnim->Duration = duration;
			pAnim->Speed = 1.0;

			m_pMesh->addSkeletalAnimation(pAnim, false);
		}
	}

	void GLTFIO::readSkinningData() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;
		std::map<int, T3DMesh<float>::Bone*> bones_by_indices;

		for (Skin skin : m_model.skins) {
			std::vector<std::vector<float>> inverseBindMatrices;

			getAccessorData(skin.inverseBindMatrices, &inverseBindMatrices);

			std::vector<Eigen::Matrix4f> offsetMatrices;

			toMat4f(&inverseBindMatrices, &offsetMatrices);

			int counter = 0;

			for (int i : skin.joints) {
				auto pBone = new T3DMesh<float>::Bone;
				pBone->OffsetMatrix = offsetMatrices[counter];
				pBone->ID = i;
				pBones->push_back(pBone);

				bones_by_indices.emplace(i, pBone);

				counter++;
			}

			//link bones together
			for (int i = 0; i < pBones->size(); i++) {
				auto pBone = (*pBones)[i];
				auto boneNode = m_model.nodes[pBone->ID];

				for (int32_t c : boneNode.children) {
					auto childBone = bones_by_indices[c];

					pBone->Children.push_back(childBone);

					childBone->pParent = pBone;
				}
			}
		}

		m_pMesh->bones(pBones, false);
	}

	void GLTFIO::readMorphTargets() {
		int offset_index = 0;

		for (auto m : m_model.meshes) {
			for (auto p : m.primitives) {
				for (auto t : p.targets) {
					T3DMesh<float>::MorphTarget* pMorphTarget = new T3DMesh<float>::MorphTarget;

					pMorphTarget->ID = offset_index;

					int32_t last_index_buff_view = -1;

					int32_t index_offset = 0;
					for (int i = 0; i < offset_index; i++) index_offset += m_offsets[i];

					for (auto keyValuePair : t) {
						std::vector<std::vector<float>> data;
						std::vector<Eigen::Vector3f> attribute_offsets;
						std::vector<int32_t> indices;

						int32_t index_buff_view = getSparseAccessorData(keyValuePair.second, &indices, &data);

						//Accessor is not sparse
						if (index_buff_view == -1) {
							getAccessorData(keyValuePair.second, &data);
							for (int i = 0; i < data.size(); i++) indices.push_back(i);
						}

						for (int i = 0; i < indices.size(); i++) indices[i] += index_offset;

						toVec3f(&data, &attribute_offsets);

						if (keyValuePair.first == "POSITION") {
							pMorphTarget->VertexOffsets = attribute_offsets;
							pMorphTarget->VertexIDs = indices;
						}
						else if (keyValuePair.first == "NORMAL") {
							pMorphTarget->NormalOffsets = attribute_offsets;
							pMorphTarget->VertexIDs = indices;

						}
					}

					if (pMorphTarget->VertexIDs.size()) m_pMesh->addMorphTarget(pMorphTarget, false);
				}

				offset_index += 1;
			}
		}
	}

	/*
	Every node in gltf has a reference as submesh and some nodes also as bone in CrossForge,
	because a gltf node can fullfill both rolls.
	Bones are read in readSkinningData.
	*/
	void GLTFIO::readNodes() {
		std::vector<T3DMesh<float>::Bone*>* pBones = new std::vector<T3DMesh<float>::Bone*>;

		for (int i = 0; i < m_model.nodes.size(); i++) {
			Node node = m_model.nodes[i];

			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh;

			if (node.matrix.size()) {
				auto mat = toSingleMat4(&node.matrix);

				pSubmesh->TranslationOffset = getTranslation(mat);
				pSubmesh->RotationOffset = getRotation(mat);

				auto scale = getScale(mat);

				if (std::abs(scale(0) - 1.0) > 0.001 || std::abs(scale(1) - 1.0) > 0.001 || std::abs(scale(2) - 1.0) > 0.001) {
					std::cout << "SCALE FOUND!!!" << std::endl;
				}
			}
			else {
				if (node.translation.size()) {
					pSubmesh->TranslationOffset(0) = node.translation[0];
					pSubmesh->TranslationOffset(1) = node.translation[1];
					pSubmesh->TranslationOffset(2) = node.translation[2];
				}

				if (node.rotation.size()) {
					Eigen::Quaternionf newRotation(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
					pSubmesh->RotationOffset = newRotation;
				}
			}

			//TODO: add Scale field to submesh struct

			pSubmesh->pParent = nullptr;

			m_pMesh->addSubmesh(pSubmesh, false);
		}


		//Do a second pass to link all submeshes together.

		for (int i = 0; i < m_model.nodes.size(); i++) {
			std::vector<T3DMesh<float>::Bone*> children;
			std::vector<T3DMesh<float>::Submesh*> submeshChildren;

			auto pSubmesh = m_pMesh->getSubmesh(i);

			for (auto c : m_model.nodes[i].children) {
				auto pSubChild = m_pMesh->getSubmesh(c);

				pSubChild->pParent = pSubmesh;

				submeshChildren.push_back(pSubChild);
			}
			pSubmesh->Children = submeshChildren;
		}
	}
#pragma endregion

}//CForge
