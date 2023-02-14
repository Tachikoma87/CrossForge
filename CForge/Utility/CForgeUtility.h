/*****************************************************************************\
*                                                                           *
* File(s): CForgeUtility.h and CForgeUtility.cpp                               *
*                                                                           *
* Content: Various utility methods.                   *
*                *
*                               *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_CFORGEUTILITY_H__
#define __CFORGE_CFORGEUTILITY_H__

#include <chrono>
#include "../Core/CForgeObject.h"
#include "../Core/CoreDefinitions.h"
#include "../AssetIO/T2DImage.hpp"
#include "../AssetIO/T3DMesh.hpp"
#include "../Graphics/Font/Font.h"

namespace CForge {
	class CFORGE_API CForgeUtility : public CForgeObject {
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
			int32_t MaxColorAttachements;

			int32_t MaxUniformBlockSize;
			int32_t MaxVaryingVectors;

			int32_t MaxVertexAttribs;

			int32_t GLMinorVersion;
			int32_t GLMajorVersion;
			std::string GLVersion;
		};

		enum DefaultMaterial : uint8_t {
			METAL_GOLD = 0,
			METAL_SILVER,
			METAL_COPPER,
			METAL_IRON,
			METAL_STEEL,
			METAL_STAINLESS_STEEL,
			METAL_WHITE,
			METAL_RED,
			METAL_BLUE,
			METAL_GREEN,

			PLASTIC_WHITE,
			PLASTIC_GREY,
			PLASTIC_BLACK,
			PLASTIC_RED,
			PLASTIC_GREEN,
			PLASTIC_BLUE,
			PLASTIC_YELLOW,

			STONE_WHITE,
			STONE_GREY,
			STONE_BLACK,
			STONE_RED,
			STONE_GREEN,
			STONE_BLUE,
			STONE_YELLOW,

			DEFAULT_MATERIAL_COUNT,
		};//DefaultMaterial

		enum DefaultFontType : int8_t {
			FONTTYPE_SANSERIF,
			FONTTYPE_SERIF,
			FONTTYPE_MONO,
			FONTTYPE_HANDWRITING,
		};//DefaultFont

		static uint64_t timestamp(void) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}//timestamp

		template<typename T>
		static void memset(T* pMemory, T Value, uint32_t Count) {
			for (uint32_t i = 0; i < Count; ++i) pMemory[i] = Value;
		}//memset

		static std::string toLowerCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::tolower(Rval[i]);
			return Rval;
		}//toLowerCase

		static std::string toUpperCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::toupper(Rval[i]);
			return Rval;
		}//toUpperCase

		static void retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg);
		static void retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near = -1.0f, float Far = -1.0f);
		static void retrieveFrameBuffer(T2DImage<uint8_t>* pColor, T2DImage<uint8_t>* pDepth = nullptr, float Near = -1.0f, float Far = -1.0f);

		static uint32_t checkGLError(std::string* pVerbose);
		static uint32_t gpuMemoryAvailable(void);
		static uint32_t gpuFreeMemory(void);

		static GPUTraits retrieveGPUTraits(void);

		static void defaultMaterial(T3DMesh<float>::Material* pMat, DefaultMaterial M);
		static void defaultMaterial(class RenderMaterial* pMat, DefaultMaterial M);

		static std::string convertToString(const std::u32string String);
		static std::u32string convertTou32String(const std::string String);

		static Font* defaultFont(DefaultFontType FontType, uint32_t FontSize, bool Bold = false, bool Italic = false, std::u32string CharSet = U"");

		CForgeUtility(void);
		~CForgeUtility(void);
	protected:

		struct MaterialDefinition {
			Eigen::Vector4f Color;
			float Metallic;
			float Roughness;
		};

		static MaterialDefinition retrieveMaterailDefinition(DefaultMaterial Mat);

	};//CForgeUtility

}//name space

#endif 