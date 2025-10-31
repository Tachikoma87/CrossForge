/*****************************************************************************\
*                                                                           *
* File(s): CForgeUtility.h and CForgeUtility.cpp                            *
*                                                                           *
* Content: Class with Various utility methods.                              *
*                                                                           *
*                                                                           *
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

	/**
	* \brief Class which offers various utility methods.
	* \ingroup Utility
	* 
	* \todo Write documentation concept page on how to avoid pointer arithmetic using smart pointer and standard containers.
	* \todo Add capability to read monitor setup and make it work with window class showing window on specific monitor.
	* \todo Check GPU memory methods whether they work on Nvidia and Amd GPUs correctly.
	*/
	class CFORGE_API CForgeUtility : public CForgeObject {
	public:

		/**
		* \brief Structure that stores traits and capabilities of the graphics processing units.
		*/
		struct GPUTraits {
			int32_t MaxTextureImageUnits;		///< Maximum number of texture image units.
			int32_t MaxVertexUniformBlocks;		///< Maximum number of vertex uniform blocks.
			int32_t MaxVertexUniformComponents;	///< Maximum number of vertex uniform components.
			int32_t MaxFragmentUniformBlocks;	///< Maximum number of fragment uniform blocks.
			int32_t MaxFragmentUniformComponents;	///< Maximum number of fragment uniform components.
			int32_t MaxGeometryUniformComponents;	///< Maximum number of geometry uniform components.

			int32_t MaxFramebufferWidth;	///< Maximum framebuffer width.
			int32_t MaxFramebufferHeight;	///< Maximum framebuffer height.
			int32_t MaxColorAttachements;	///< Maximum framebuffer object color attachements.

			int32_t MaxUniformBlockSize;	///< Maximum uniform block size.
			int32_t MaxVaryingVectors;		///< Maximum varying vectors.

			int32_t MaxVertexAttribs;	///< Maximum of vertex attributes.

			int32_t GLMinorVersion;		///< OpenGL minor version.
			int32_t GLMajorVersion;		///< OpenGL major version.
			std::string GLVersion;		///< OpenGL version as string.
		};

		/**
		* \brief Defines for available default materials.
		*/
		enum DefaultMaterial : int8_t {
			MATERIAL_UNKNOWN = -1,	///< Default value.
			METAL_GOLD = 0,	///< Metal gold.
			METAL_SILVER,	///< Metal silver.
			METAL_COPPER,	///< Metal copper.
			METAL_IRON,		///< Metal iron.
			METAL_STEEL,	///< Metal steel.
			METAL_STAINLESS_STEEL,	///< Metal stainless steel.
			METAL_WHITE,	///< Metal white.
			METAL_RED,		///< Metal red.
			METAL_BLUE,		///< Metal blue.
			METAL_GREEN,	///< Metal green.

			PLASTIC_WHITE,	///< Plastic white.
			PLASTIC_GRAY,	///< Plastic gray.
			PLASTIC_BLACK,	///< Plastic black.
			PLASTIC_RED,	///< Plastic red.
			PLASTIC_GREEN,	///< Plastic green.
			PLASTIC_BLUE,	///< Plastic blue.
			PLASTIC_YELLOW,	///< Plastic yellow.

			STONE_WHITE,	///< Tone white.
			STONE_GRAY,		///< Stone gray.
			STONE_BLACK,	///< Stone black.
			STONE_RED,		///< Stone red.
			STONE_GREEN,	///< Stone green.
			STONE_BLUE,		///< Stone blue.
			STONE_YELLOW,	///< Stone yellow.

			DEFAULT_MATERIAL_COUNT,	///< Number of material defines.
		};//DefaultMaterial

		/**
		* \brief Defines for available default fonts.
		*/
		enum DefaultFontType : int8_t {
			FONTTYPE_UNKNOWN = -1,	///< Default value.
			FONTTYPE_SANSERIF = 0,	///< Font without serifs.
			FONTTYPE_SERIF,			///< Font with serif.
			FONTTYPE_MONO,			///< Mono font.
			FONTTYPE_HANDWRITING,	///< Handwriting font.

			FONTTYPE_COUNT			///< Number of default fonts.
		};//DefaultFont

		/**
		* \brief Getter for a timestamp, i.e. milliseconds past since January 1st 1970
		* 
		* \return Milliseconds past since January 1st 1970
		*/
		static uint64_t timestamp(void) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}//timestamp

		/**
		* \brief Sets a chunk of memory (array) to a default value.
		* 
		* \param[in,out] pMemory Buffer.
		* \param[in] Value New default value.
		* \param[in] Count Number of elements in the buffer.
		*/
		template<typename T>
		static void memset(T* pMemory, T Value, uint32_t Count) {
			for (uint32_t i = 0; i < Count; ++i) pMemory[i] = Value;
		}//memset

		/**
		* \brief Turns a given string into the lower case version.
		* 
		* \return Lower case version of the string.
		*/
		static std::string toLowerCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::tolower(Rval[i]);
			return Rval;
		}//toLowerCase

		/**
		* \brief Turns a given string into its upper case version.
		* 
		* \return Upper case version of the string.
		*/
		static std::string toUpperCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::toupper(Rval[i]);
			return Rval;
		}//toUpperCase

		/**
		* \brief Retrieve the image data of a color texture.
		* 
		* \param[in] TexObj OpenGL texture object identifier. Has to be an existing texture object.
		* \param[out] pImg Image data will be written to this object.
		* \throws CrossForgeException Throws exception if texure object is not valid.
		*/
		static void retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg);

		/**
		* \brief Retrieves the image data of an OpenGL texture object interpreted as depth values.
		* 
		* If Near and Far plane values specified greater 0 the depth values will we linearized. 
		* 
		* \param[in] TexObj OpenGL texture object identifier. Has to be an existing texture object.
		* \param[out] pImg Image data will be written to this object.
		* \param[in] Near Clipping plane near value used for linearizion of the depth values.
		* \param[in] Far Clipping plane far value used for linearizion of the depth values.
		*/
		static void retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near = -1.0f, float Far = -1.0f);

		/**
		* \brief Reads the OpenGL back buffer (color and depth).
		* 
		* \param[out] pColor Color buffer will be stored here. Can be nullptr.
		* \param[out] pDepth Depth map values will be stored here. Can be nullptr.
		* \param[in] Near Clipping plane near value used for linearizion of depth values.
		* \param[in] Far Clipping plane far value used for linearizion of depth values.
		*/
		static void retrieveFrameBuffer(T2DImage<uint8_t>* pColor, T2DImage<uint8_t>* pDepth = nullptr, float Near = -1.0f, float Far = -1.0f);

		/**
		* \brief Check whether an OpenGL error has occurred.
		* 
		* \param[out] pVerbose Written explanation of the occurred error, if any. String is empty if no error occurred.
		* \return Error code equal to OpenGL error code. 0 if no error occurred.
		*/
		static uint32_t checkGLError(std::string* pVerbose);

		/**
		* \brief Getter for total available video memory.
		* 
		* \return Total amount of video memory on the system in bytes. 0 if method is not supported.
		* \warning May not be available on any system configuration.
		*/
		static uint32_t gpuMemoryAvailable(void);

		/**
		* \brief Getter for free amount of video memory.
		* 
		* \return Free video memory left in bytes. 0 if method is not supported.
		* \warning May not be available on any system configuration.
		*/
		static uint32_t gpuFreeMemory(void);

		/**
		* \brief Reads several capabilities of the GPU and returns the filled structure. \see GPUTraits
		* 
		* \return Structure that contains the GPU traits.
		*/
		static GPUTraits retrieveGPUTraits(void);

		/**
		* \brief Creates a default material.
		* 
		* \param[out] Material to create.
		* \param[in] M Default material identifier.
		*/
		static void defaultMaterial(T3DMesh<float>::Material* pMat, DefaultMaterial M);

		/**
		* \brief Creates a default material.
		* 
		* \param[out] pMat Material to create.
		* \param[in] M Default material identifier.
		*/
		static void defaultMaterial(class RenderMaterial* pMat, DefaultMaterial M);

		/**
		* \brief Converts a u32 string to a regular string.
		* 
		* \param[in] String u32 input string.
		* \return Converted string.
		*/
		static std::string convertToString(const std::u32string String);

		/**
		* \brief Converts a wide string to a regular one.
		* 
		* \param[in] String Input wide string.
		* \return Converted string.
		*/
		static std::string convertToString(const std::wstring String);

		/**
		* \brief Converts a regular string to a u32 string.
		* 
		* \param[in] String The input string.
		* \return Converted u32 string.
		*/
		static std::u32string convertTou32String(const std::string String);

		/**
		* \brief Converts a regular string to a wide string.
		* 
		* \param[in] String Input string to convert.
		* \return Converted string.
		*/
		static std::wstring convertToWString(const std::string String);

		/**
		* \brief Creates a default font with the specified traits.
		* 
		* \param[in] FontType Type of the font.
		* \param[in] FontSize Size of the font.
		* \param[in] Bold Whether font should appear in bold style.
		* \param[in] Italic Whether font should appear in italic style.
		* \param[in] CharSet The character set to use. If empty the default character set will be used. \see Font::FontStyle
		* \return Font handle.
		*/
		static Font* defaultFont(DefaultFontType FontType, uint32_t FontSize, bool Bold = false, bool Italic = false, std::u32string CharSet = U"");

		/**
		* \brief Converts an RGB color vector to a grayscale value. Values have to be \f$ \in [0,1] \f$.
		* 
		* \param[in] C RGB color vector.
		* \return Grayscale value.
		*/
		static float rgbToGrayscale(const Eigen::Vector3f C) {
			return rgbToGrayscale(C.x(), C.y(), C.z());
		}

		/**
		* \brief Converts RGB values into grayscale representation. Values have to be \$f \in [0,1] \$f.
		* 
		* \param[in] R Red value.
		* \param[in] G Green value.
		* \param[in] B Blue value.
		* \return Grayscale value.
		*/
		static float rgbToGrayscale(float R, float G, float B) {
			return 0.299 * R + 0.587 * G + 0.114 * B;
		}

		/**
		* \brief Constructor
		*/
		CForgeUtility(void);

		/**
		* \brief Destructor
		*/
		~CForgeUtility(void);
	protected:

		/**
		* \brief Internal structure to define a material.
		*/
		struct MaterialDefinition {
			Eigen::Vector4f Color;	///< Color value (RGBA).
			float Metallic;			///< Metallic component.
			float Roughness;		///< Roughness component.
		};

		/**
		* \brief Creates the material definition of the default materials.
		* 
		* \param[in] Mat Default material identifier.
		* \return Material definition.
		*/
		static MaterialDefinition retrieveMaterailDefinition(DefaultMaterial Mat);

	};//CForgeUtility

}//name space

#endif 