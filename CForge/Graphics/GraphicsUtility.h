/*****************************************************************************\
*                                                                           *
* File(s): GraphicsUtility.h and GraphicsUtility.cpp                        *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_GRAPHICSUTILITY_H__
#define __CFORGE_GRAPHICSUTILITY_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T2DImage.hpp"

namespace CForge {
	/**
	* \brief Support methods for working with graphics.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API GraphicsUtility: public CForgeObject {
	public:
		struct GPUTraits {
			int32_t MaxTextureImageUnits;
			int32_t MaxVertexUniformBlocks;
			int32_t MaxVertexUniformComponents;
			int32_t MaxFragmentUniformBLocks;
			int32_t MaxFragmentUniformComponents;
			int32_t MaxGeometryUniformComponents;

			int32_t MaxFramebufferWidth;
			int32_t MaxFramebufferHeight;

			int32_t MaxUniformBlockSize;
			int32_t MaxVaryingVectors;

			int32_t MaxVertexAttribs;

			int32_t GLMinorVersion;
			int32_t GLMajorVersion;
			std::string GLVersion;
		};

		GraphicsUtility(void);
		~GraphicsUtility(void);

		void init(void);
		void clear(void);

		static Eigen::Matrix4f perspectiveProjection(uint32_t Width, uint32_t Height, float FieldOfView, float Near, float Far);
		static Eigen::Matrix4f perspectiveProjection(float Left, float Right, float Bottom, float Top, float Near, float Far);
		static Eigen::Matrix4f orthographicProjection(float Left, float Right, float Bottom, float Top, float Near, float Far);
		static Eigen::Matrix4f orthographicProjection(float Right, float Top, float Near, float Far);
		static Eigen::Matrix4f lookAt(Eigen::Vector3f Position, Eigen::Vector3f Target, Eigen::Vector3f Up = Eigen::Vector3f::UnitY());
		static void asymmetricFrusti(uint32_t Width, uint32_t Height, float Near, float Far, float FOV, float FocalLength, float EyeSep, Eigen::Matrix4f* pLeftEye, Eigen::Matrix4f* pRightEye);

		static Eigen::Matrix4f rotationMatrix(Eigen::Quaternionf Rot);
		static Eigen::Matrix4f translationMatrix(Eigen::Vector3f Trans);
		static Eigen::Matrix4f scaleMatrix(Eigen::Vector3f Scale);

		static Eigen::Matrix3f alignVectors(const Eigen::Vector3f Source, const Eigen::Vector3f Target);

		static void retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg);
		static void retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near = -1.0f, float Far = -1.0f);
		static void retrieveFrameBuffer(T2DImage<uint8_t>* pColor, T2DImage<uint8_t>* pDepth = nullptr, float Near = -1.0f, float Far = -1.0f);

		static uint32_t checkGLError(std::string* pVerbose);
		static uint32_t gpuMemoryAvailable(void);
		static uint32_t gpuFreeMemory(void);

		static GPUTraits retrieveGPUTraits(void);

		template<typename T>
		static T degToRad(T Deg) {
			return Deg * T(EIGEN_PI) / T(180);
		}//degToRad

		template<typename T>
		static T radToDeg(T Rad) {
			return Rad * T(180) / T(EIGEN_PI);
		}//radToDeg

	protected:

	};//GraphicsUtility
}//name space


#endif