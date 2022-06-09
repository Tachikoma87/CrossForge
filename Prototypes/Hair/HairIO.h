/*****************************************************************************\
*                                                                           *
* File(s): HairIO.h and HairIO.cpp                                                       *
*                                                                           *
* Content:                                                                  *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann, Sascha Jüngel                                     *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_HAIRIO_H__
#define __CFORGE_HAIRIO_H__

#include "I3DHairIO.h"

namespace CForge {
	/***
	* \brief 3D hair import/export plugin.
	*
	* \todo Do full documentation
	*/
	class HairIO : public I3DHairIO {
	public:
		HairIO(void);
		~HairIO(void);

		void init(void);
		void clear(void);

		void release(void);
		bool accepted(const std::string Filepath, Operation Op);

		void load(const std::string Filepath, T3DHair<float>* pHair);
		void store(const std::string Filepath, const T3DHair<float>* pHair);

	protected:
		void loadCyHairFile(const std::string Filepath, T3DHair<float>* pHair);
		void loadTFXFile(const std::string Filepath, T3DHair<float>* pHair);
		float* computeStrandTangents(float* tangentArray, std::vector<Eigen::Matrix<float, 3, 1>> strandPoints, int currentArrayIndex, int numStrandPoints);

	};//HairIO
}//name space

#endif