/*****************************************************************************\
*                                                                           *
* File(s): I3DMeshIO.h                                     *
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
#ifndef __CFORGE_I3DMESHIO_H__
#define __CFORGE_I3DMESHIO_H__

#include "../Core/CForgeObject.h"
#include "T3DMesh.hpp"

namespace CForge {

	/**
	* \brief Interface definition for mesh import/export plug-ins
	* 
	* \todo Do full documentation
	*/

	class I3DMeshIO: public CForgeObject {
	public:
		enum Operation: uint8_t {
			OP_LOAD = 0,
			OP_STORE,
		};

		virtual void load(const std::string Filepath, T3DMesh<float>* pMesh) = 0;
		virtual void store(const std::string Filepath, const T3DMesh<float>* pMesh) = 0;

		virtual void release(void) = 0;
		virtual bool accepted(const std::string Filepath, Operation Op) = 0;
		virtual std::string pluginName(void)const;


	protected:
		I3DMeshIO(const std::string ClassName);
		virtual ~I3DMeshIO(void);
		std::string m_PluginName;
	};//I3DMeshIO

}//name space


#endif