#ifndef __TEMPREG_DATASETCOMMON_H__
#define __TEMPREG_DATASETCOMMON_H__

namespace TempReg {

	enum class DatasetType {
		NONE,
		TEMPLATE,		// original template
		DTEMPLATE,		// deformed template
		TARGET			// target
	};

	enum class DatasetGeometryType {
		MESH,
		POINTCLOUD
	};
}

#endif