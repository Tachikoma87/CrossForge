/*****************************************************************************\
*                                                                           *
* File(s): GraphicsUtility.h and GraphicsUtility.cpp                        *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_GRAPHICSUTILITY_H__
#define __CROSSFORGE_GRAPHICSUTILITY_H__

#include <crossforge/assetio/entities/Image2DEntity.h>
#include "../Graphics/components/actorprefab/PbrMaterialsComponent.h"

namespace crossforge {
	class GraphicsUtility {
	public:

		/**
		* \brief Structure that stores traits and capabilities of the graphics processing units.
		*/
		struct GPUTraits {
			int32_t maxTextureImageUnits;		///< Maximum number of texture image units.
			int32_t maxVertexUniformBlocks;		///< Maximum number of vertex uniform blocks.
			int32_t maxVertexUniformComponents;	///< Maximum number of vertex uniform components.
			int32_t maxFragmentUniformBlocks;	///< Maximum number of fragment uniform blocks.
			int32_t maxFragmentUniformComponents;	///< Maximum number of fragment uniform components.
			int32_t maxGeometryUniformComponents;	///< Maximum number of geometry uniform components.

			int32_t maxFramebufferWidth;	///< Maximum framebuffer width.
			int32_t maxFramebufferHeight;	///< Maximum framebuffer height.
			int32_t maxColorAttachements;	///< Maximum framebuffer object color attachements.

			int32_t maxUniformBlockSize;	///< Maximum uniform block size.
			int32_t maxVaryingVectors;		///< Maximum varying vectors.

			int32_t maxVertexAttribs;	///< Maximum of vertex attributes.

			int32_t glMinorVersion;		///< OpenGL minor version.
			int32_t glLMajorVersion;		///< OpenGL major version.
			std::string glVersion;		///< OpenGL version as string.
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
		* \brief Retrieve the image data of a color texture.
		*
		* \param[in] TexObj OpenGL texture object identifier. Has to be an existing texture object.
		* \param[out] pImg Image data will be written to this object.
		* \throws CrossForgeException Throws exception if texure object is not valid.
		*/
		static bool retrieveColorTexture(uint32_t texObj, Image2DEntityPtr pImage, uint32_t level);

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
		static bool retrieveDepthTexture(uint32_t texObj, Image2DEntityPtr pImage, uint32_t level, float near = -1.0f, float far = -1.0f);

		/**
		* \brief Reads the OpenGL back buffer (color and depth).
		*
		* \param[out] pColor Color buffer will be stored here. Can be nullptr.
		* \param[out] pDepth Depth map values will be stored here. Can be nullptr.
		* \param[in] Near Clipping plane near value used for linearizion of depth values.
		* \param[in] Far Clipping plane far value used for linearizion of depth values.
		*/
		static bool retrieveFrameBuffer(Image2DEntityPtr pColorImage, Image2DEntityPtr pDepthImage = nullptr, float near = -1.0f, float far = -1.0f);


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
		* \param[out] pMat Material to create.
		* \param[in] M Default material identifier.
		*/
		static void createDefaultMaterial(PbrMaterialPtr pMaterial, DefaultMaterial mat);

		/**
		* \brief Check whether an OpenGL error has occurred.
		*
		* \param[out] pVerbose Written explanation of the occurred error, if any. String is empty if no error occurred.
		* \return Error code equal to OpenGL error code. 0 if no error occurred.
		*/
		static uint32_t checkGLError(std::string* pVerbose);

	protected:
		GraphicsUtility();
		~GraphicsUtility();

	};

}

#endif 