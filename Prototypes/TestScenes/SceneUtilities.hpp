/*****************************************************************************\
*                                                                           *
* File(s): SceneUtilities.hpp                                            *
*                                                                           *
* Content: Class to interact with an MF52 NTC Thermistor by using a basic   *
*          voltage divider circuit.                                         *
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
#ifndef __CFORGE_SCENEUTILITIES_HPP__
#define __CFORGE_SCENEUTILITIES_HPP__

namespace CForge {

	void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
		for (uint32_t i = 0; i < pM->materialCount(); ++i) {
			T3DMesh<float>::Material* pMat = pM->getMaterial(i);
			pMat->VertexShaderSources.push_back("Shader/BasicGeometryPass.vert");
			pMat->FragmentShaderSources.push_back("Shader/BasicGeometryPass.frag");
			pMat->Metallic = Metallic;
			pMat->Roughness = Roughness;
		}//for[materials]
	}//setMeshShader

}//name space

#endif 