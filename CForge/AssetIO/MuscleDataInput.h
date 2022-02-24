
#include <vector>
#include <string>

namespace CForge {

	/**
	* \brief This singleton class handles import and export of meshes and image files.
	*
	* \todo Do full documentation
	* \todo Design and implement mesh file format for crossforge
	* \todo Implement TImagePyramid (handles image pyramid with different resolutions)
	* \todo implement DDS import/export
	*/

	class  MuscleDataInput  {
	public:
		struct Muscledata
		{
			std::string name;
			std::vector<float>  data;
		};

		struct Muscle_allocdata {
			std::string name;
			float min;
			float max;
		};

		static void loadDataFile(char const* file, std::vector<Muscledata>* _muscledata,  int* _frames);
		static void loadMuscleAllocation(char const* file, std::vector<Muscle_allocdata>* _ma_data);

		

	
	};//MuscleDataInput

	typedef MuscleDataInput MuscleDataInput;

}//name space


