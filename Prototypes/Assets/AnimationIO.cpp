#include "AnimationIO.h"
#include "../../CForge/AssetIO/File.h"

using namespace Eigen;

namespace CForge {

	uint32_t stringToStream(std::string S, uint8_t* pStream) {
		uint32_t Rval = 0;
		// write number of chars
		uint32_t L = S.length() * sizeof(uint8_t);
		memcpy(&pStream[0], &L, sizeof(uint32_t)); Rval += sizeof(uint32_t);

		memcpy(&pStream[Rval], S.c_str(), S.length() * sizeof(uint8_t));
		Rval += S.length() * sizeof(uint8_t);
		return Rval;
	}//stringToStream

	std::string streamToString(uint8_t* pStream) {
		// read number of chars
		uint8_t Buffer[64];
		
		uint32_t L = 0;
		memcpy(&L, &pStream[0], sizeof(uint32_t));
		memcpy(Buffer, &pStream[4], L);
		Buffer[L] = '\0';
		std::string Rval = (char*)Buffer;
		return Rval;

	}//stream

	void AnimationIO::storeSkeletalAnimation(std::string Filepath, T3DMesh<float>* pMesh, uint32_t startIndex, uint32_t endIndex) {
		File F;
		F.begin(Filepath, "wb");
		if (!F.valid()) throw CForgeExcept("File " + Filepath + " can not be opened in wb mode");

		uint8_t* pBuffer = new uint8_t[1024];

		// write number of animation
		//uint32_t AnimCount = pMesh->skeletalAnimationCount();
		uint32_t AnimCount = endIndex - startIndex + 1;
		F.write(&AnimCount, sizeof(uint32_t));

		for (uint32_t i = startIndex; i <= endIndex; ++i) {
			auto* pSkelAnim = pMesh->getSkeletalAnimation(i);

			// write name
			uint32_t L = stringToStream(pSkelAnim->Name, pBuffer);
			F.write(&L, sizeof(uint32_t));
			F.write(pBuffer, L);
			// write duration
			F.write(&pSkelAnim->Duration, sizeof(float));
			// write speed
			F.write(&pSkelAnim->Speed, sizeof(float));

			L = pSkelAnim->Keyframes.size();
			// write number of keyframes
			F.write(&L, sizeof(uint32_t));

			// write keyframes
			for (uint32_t k = 0; k < pSkelAnim->Keyframes.size(); ++k) {
				auto* pKeyframe = pSkelAnim->Keyframes[k];

				// write boneID
				F.write(&pKeyframe->BoneID, sizeof(int32_t));
				// write ID
				F.write(&pKeyframe->ID, sizeof(int32_t));

				uint32_t B = stringToStream(pKeyframe->BoneName, pBuffer);
				F.write(&B, sizeof(uint32_t));
				F.write(pBuffer, B);
				//does it need BoneName?????

				// write skinning matrix (acutally unused at ath moment)
				//F.write(pKeyframe->SkinningMatrix.data(), sizeof(float) * 16);

				//write position data
				uint32_t PosCount = pKeyframe->Positions.size();
				F.write(&PosCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < PosCount; ++j) F.write(pKeyframe->Positions[j].data(), sizeof(float) * 3);

				// write rotation data
				uint32_t RotCount = pKeyframe->Rotations.size();
				F.write(&RotCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < RotCount; ++j) {
					F.write(&pKeyframe->Rotations[j].w(), sizeof(float));
					F.write(&pKeyframe->Rotations[j].x(), sizeof(float));
					F.write(&pKeyframe->Rotations[j].y(), sizeof(float));
					F.write(&pKeyframe->Rotations[j].z(), sizeof(float));

				}

				// write scale data
				uint32_t ScaleCount = pKeyframe->Scalings.size();
				F.write(&ScaleCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < ScaleCount; ++j) F.write(pKeyframe->Scalings[j].data(), sizeof(float) * 3);

				// write timestamps
				uint32_t TimestampsCount = pKeyframe->Timestamps.size();
				F.write(&TimestampsCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < TimestampsCount; ++j) F.write(&pKeyframe->Timestamps[j], sizeof(float));
			}//for[keyframes]
		}//for[animations]

		F.end();

		delete[] pBuffer;
	}//storeSkeletalAnimation

	void AnimationIO::loadSkeletalAnimation(std::string Filepath, T3DMesh<float>* pMesh) {

		File F;
		F.begin(Filepath, "rb");

		if (!F.valid()) throw CForgeExcept("File " + Filepath + " could not be opened in read mode!");
		uint8_t* pBuffer = new uint8_t[1024];

		// read number of skeletal animtions
		uint32_t SkelAnimCount = 0;
		F.read(&SkelAnimCount, sizeof(uint32_t));

		for (uint32_t i = 0; i < SkelAnimCount; ++i) {
			T3DMesh<float>::SkeletalAnimation* pSkelAnim = new T3DMesh<float>::SkeletalAnimation();

			// read name
			uint32_t L = 0;
			F.read(&L, sizeof(uint32_t));
			F.read(pBuffer, L);
			pSkelAnim->Name = streamToString(pBuffer);

			// read duration
			F.read(&pSkelAnim->Duration, sizeof(float));
			// read speed
			F.read(&pSkelAnim->Speed, sizeof(float));

			// read number of keyframes
			uint32_t KeyframeCount = 0;
			F.read(&KeyframeCount, sizeof(uint32_t));

			float Values[4];

			for (uint32_t k = 0; k < KeyframeCount; ++k) {
				T3DMesh<float>::BoneKeyframes* pKeyframe = new T3DMesh<float>::BoneKeyframes();

				// bone id
				F.read(&pKeyframe->BoneID, sizeof(int32_t));
				// id
				F.read(&pKeyframe->ID, sizeof(int32_t));

				uint32_t B = 0;
				F.read(&B, sizeof(uint32_t));
				F.read(pBuffer, B);
				pKeyframe->BoneName = streamToString(pBuffer);

				// position data
				uint32_t PosCount = 0;
				F.read(&PosCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < PosCount; ++j) {
					F.read(&Values[0], uint64_t(sizeof(float) * 3));
					pKeyframe->Positions.push_back(Vector3f(Values[0], Values[1], Values[2]));
				}

				// rotation data
				uint32_t RotCount = 0;
				F.read(&RotCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < RotCount; ++j) {
					F.read(Values, sizeof(float) * 4);
					pKeyframe->Rotations.push_back(Quaternionf(Values[0], Values[1], Values[2], Values[3]));
				}

				// scaling data
				uint32_t ScalingCount = 0;
				F.read(&ScalingCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < ScalingCount; ++j) {
					F.read(Values, sizeof(float) * 3);
					pKeyframe->Scalings.push_back(Vector3f(Values[0], Values[1], Values[2]));
				}

				// read timestamps
				uint32_t TimestampsCount = 0;
				F.read(&TimestampsCount, sizeof(uint32_t));
				for (uint32_t j = 0; j < TimestampsCount; ++j) {
					F.read(Values, sizeof(float));
					pKeyframe->Timestamps.push_back(Values[0]);
				}

				pSkelAnim->Keyframes.push_back(pKeyframe);
			}//for[keyframes]

			pMesh->addSkeletalAnimation(pSkelAnim, false);
		}

		F.end();

		delete[] pBuffer;
	}//loadSkeletalAnimation


	AnimationIO::AnimationIO(void) {

	}//Constructor

	AnimationIO::~AnimationIO(void) {

	}//Destructor

}//namespace