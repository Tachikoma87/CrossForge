/*****************************************************************************\
*                                                                           *
* File(s): I2DImageIO.h and I2DImageIO.cpp                                     *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_I2DIMAGEIO_H__
#define __CFORGE_I2DIMAGEIO_H__

#include "../Core/CForgeObject.h"
#include "T2DImage.hpp"


namespace CForge {

	/**
	* \brief Interface definition for image import/export plug-ins 
	* 
	* \todo Do full documentation
	*
	*/

	class CFORGE_API I2DImageIO: public CForgeObject {
	public:

		enum Operation : uint8_t {
			OP_LOAD = 0,
			OP_STORE,
		};

		virtual void load(const std::string Filepath, T2DImage<uint8_t>* pImgData) = 0;
		virtual void store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) = 0;
		virtual bool accepted(const std::string Filepath, Operation Op) = 0;
		virtual void release(void) = 0;

		virtual std::string pluginName(void)const;

	protected:
		I2DImageIO(const std::string ClassName);
		virtual ~I2DImageIO(void);

		std::string m_PluginName;
	private:

	};//I2DImageIO

}//name space

#endif