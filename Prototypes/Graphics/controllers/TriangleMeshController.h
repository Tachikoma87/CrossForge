/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshController.h and TriangleMeshController.cpp                     *
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
#ifndef __CROSSFORGE_TRIANGLEMESHCONTROLLER_H__
#define __CROSSFORGE_TRIANGLEMESHCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include <crossforge/assetio/entities/TriangleMeshEntity.h>

namespace crossforge {
	class TriangleMeshController : public ControllerBase {
	public:
		static inline std::string identification = "TriangleMeshController";

		static bool recomputeVertexNormals(TriangleMeshEntityPtr pTriangleMesh);

		~TriangleMeshController();
	protected:
		TriangleMeshController(const std::string childIdentification);
	};

	typedef std::shared_ptr<TriangleMeshController> TriangleMeshControllerPtr;
}

#endif 