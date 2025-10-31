/*****************************************************************************\
*                                                                           *
* File(s): UBOLightData.h and UBOLightData.cpp                              *
*                                                                           *
* Content: Uniform buffer object for light data.                            *
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
#ifndef __CFORGE_UBOLIGHTDATA_H__
#define __CFORGE_UBOLIGHTDATA_H__

#include "../Lights/ILight.h"
#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for light related data.
	* \ingroup UniformBufferObjects
	* 
	* \todo Change pass by pointer to pass by reference.
	*/
	class CFORGE_API UBOLightData: CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		UBOLightData(void);

		/**
		* \brief Destructor
		*/
		~UBOLightData(void);

		/**
		* \brief Initialization method.
		* 
		* \param[in] DirectionalLights Number of directional lights in the scene.
		* \param[in] PointLights Number of point lights in the scene.
		* \param[in] SpotLights Number of spot lights in the scene.
		*/
		void init(uint32_t DirectionalLights, uint32_t PointLights, uint32_t SpotLights);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the buffer to a binding points.
		* 
		* \param[in] BindingPoint Binding point obtained from the shader.
		* \param[in] Type Light type data the binding point is associated with.
		*/
		void bind(uint32_t BindingPoint, ILight::LightType Type);

		/**
		* \brief Returns size int bytes of the buffer data of a specific light type.
		* 
		* \param[in] Type The light type for which the size should be computed.
		* \return Size if bytes of the buffer's data for a specific light type.
		*/
		uint32_t size(ILight::LightType Type);

		/**
		* \brief Returns number of available lights for a specific light type.
		* 
		* \param[in] Type Light type for which the number will be queried.
		* \return Number of lights for the specified light type.
		*/
		uint32_t lightCount(ILight::LightType Type);

		/**
		* \brief Sets the position data of a specific light.
		* 
		* \param[in] Pos Position data.
		* \param[in] Type The light type.
		* \param[in] Index Light index.
		*/
		void position(Eigen::Vector3f Pos, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the direction data of a specific light.
		* 
		* \param[in] Dir Direction data to set.
		* \param[in] Type The light type.
		* \param[in] Index Light index.
		*/
		void direction(Eigen::Vector3f Dir, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the color data of a specific light.
		* 
		* \param[in] Color The color data to set.
		* \param[in] Type The light type.
		* \param[in] Index Light index.
		*/
		void color(Eigen::Vector3f Color, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the intensity value.
		* 
		* \param[in] Intensity The new intensity value.
		* \param[in] Type The light type.
		* \param[in] Index Light index.
		*/
		void intensity(float Intensity, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the attenuation values.
		* 
		* \param[in] Attenuation The new attenuation values.
		* \param[in] Type The light type.
		* \param[in] Index The light index.
		*/
		void attenuation(Eigen::Vector3f Attenuation, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the cut off values.
		* 
		* \param[in] CutOff the new cut off values.
		* \param[in] Type The light type.
		* \param[in9 Index Light index.
		*/
		void cutOff(Eigen::Vector2f CutOff, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the light space matrix.
		* 
		* \param[in] Mat Matrix to set.
		* \param[in] Type The light type.
		* \param[in] Index The light index.
		*/
		void lightSpaceMatrix(Eigen::Matrix4f Mat, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Sets the shadow id.
		* 
		* \param[in] ID Value to set.
		* \param[in] Type The light type.
		* \param[in] Index Light index.
		*/
		void shadowID(int32_t ID, ILight::LightType Type, uint32_t Index);

		/**
		* \brief Update the whole buffer for a specific light.
		* 
		* \param[in] pLight Light object from which the values will be derived.
		* \param[in] Index The light index to write to.
		* \note Light type will be derived from the light object.
		*/
		void updateLight(ILight* pLight, uint32_t Index);

	protected:
		/**
		* \brief Structure that holds the buffer access information for the directional lights.
		*/
		struct DirectionalLightData {		
			std::vector<uint32_t> DirectionOffsets;	///< Offset for the direction values.
			std::vector<uint32_t> ColorOffsets;		///< Offsets for the color values.
			std::vector<uint32_t> IntensityOffsets;		///< Offsets for the intensity values.
			std::vector<uint32_t> LightMatricesOffset;	///< Offsets for the light matrices.
			std::vector<uint32_t> ShadowIDOffsets;		///< Offsets for the shadow ids. 
			GLBuffer Buffer;							///< OpenGL buffer object.

			/**
			* \brief Initialization method.
			* 
			* \param[in] Number of lights the buffer should hold.
			*/
			void init(uint32_t LightCount);

			/**
			* \brief Clear method.
			*/
			void clear(void);

			/**
			* \brief Getter for the number of lights.
			* 
			* \return Number of lights.
			*/
			uint32_t lightCount(void)const;

			/**
			* \brief Returns size in bytes of the buffer.
			* \return Size in bytes of the buffer.
			*/
			uint32_t size(void)const;
		};

		/**
		* \brief Structure that holds the buffer access information for point light data.
		*/
		struct PointLightData {
			std::vector<uint32_t> PositionOffsets;	///< Offset of the direction values.
			std::vector<uint32_t> IntensityOffsets;	///< Offset of the intensity values.
			std::vector<uint32_t> DirectionOffsets;	///< Offset of the direction values.
			std::vector<uint32_t> ColorOffsets;		///< Offset of the color values.
			std::vector<uint32_t> AttenuationOffsets;	///< Offset of the attenuation values.
			std::vector<uint32_t> LightMatrixOffsets;	///< Offset of the light matrix values.
			std::vector<uint32_t> ShadowIDOffsets;	///< Offset of the shadow ids.
			GLBuffer Buffer;						///< OpenGL buffer object.

			/**
			* \brief Initialization method.
			* 
			* \param[in] LightCount Number of lights the buffer should hold data for.
			*/
			void init(uint32_t LightCount);

			/**
			* \brief Clear method.
			*/
			void clear(void);

			/**
			* \brief Returns the number of lights.
			* 
			* \return Number of lights.
			*/
			uint32_t lightCount(void)const;

			/**
			* \brief Returns the size of the buffer in bytes.
			* 
			* \return Size of the buffer in bytes.
			*/
			uint32_t size(void)const;
		};

		/**
		* \brief Structure that holds the buffer access information of the spot light data.
		*/
		struct SpotLightData {
			std::vector<uint32_t> PositionOffsets;		///< Offsets of the position data.
			std::vector<uint32_t> DirectionOffsets;		///< Offsets of the direction data.
			std::vector<uint32_t> OuterCutOffOffsets;	///< Offsets of the cut off data.
			std::vector<uint32_t> InnerCutOffOffsets;	///< Offsets of the inner cut of data.
			std::vector<uint32_t> ColorOffsets;			///< Offsets of the color data.
			std::vector<uint32_t> IntensityOffsets;		///< Offsets of the intensity data.
			std::vector<uint32_t> AttenuationOffsets;	///< Offsets of the attenuation data.
			std::vector<uint32_t> LightMatrixOffsets;	///< Offsets of the light matrix data.
			std::vector<uint32_t> ShadowIDOffsets;		///< Offsets of the shadow ids.
			GLBuffer Buffer;							///< OpenGL buffer object.

			/**
			* \brief Initialization method.
			* 
			* \param[in] LightCount Number of lights the buffer should hold data for.
			*/
			void init(uint32_t LightCount);

			/**
			* \brief Clear method.
			*/
			void clear(void);

			/**
			* \brief Returns number of lights.
			* 
			* \return Number of lights the buffer can manage.
			*/
			uint32_t lightCount(void)const;

			/**
			* \brief Returns the size in bytes of the buffer.
			* 
			* \return Size in bytes of the buffer.
			*/
			uint32_t size(void)const;
		};

		DirectionalLightData m_DirLightsData;	///< Directional lights buffer data.
		PointLightData m_PointLightsData;		///< Point lights buffer data.
		SpotLightData m_SpotLightsData;			///< Spot lights buffer data.
	};//UBOLightData

}//name space

#endif 