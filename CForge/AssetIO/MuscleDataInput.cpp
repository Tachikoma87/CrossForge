#include "MuscleDataInput.h"



#include "File.h"
#include <fstream>


namespace CForge {
	
	void MuscleDataInput::loadDataFile(char const* file, std::vector<Muscledata> *_muscledata, int *_frames) {
		if (file == NULL) throw CForgeExcept("No file specified!");

		std::ifstream inFile;
		inFile.open(file);

		std::string str;
		int i = 0;
		int frames = 0;
		int str_length = str.length();
		int arr[] = { 0 };
		std::vector<Muscledata> muscledata;

		while (std::getline(inFile, str)) {
			i++;

			if (i > 12) {
				int j = 0;
				std::string word = "";
				
				for (int k = 0; str[k] != '\0'; k++) {
					if (str[k] == ' ' || str[k] == '\t') {
						if (k >= 1) {
							if (str[k - 1] != ' ' && str[k - 1] != '\t') {
								muscledata.at(j).data.push_back(std::atof(word.c_str()));
								word.erase();
								j++;
							}
						}
					}
					else word.append(str, k, 1);
				}

				muscledata.at(j).data.push_back(std::atof(word.c_str()));

			}
			else if (!str.find("nRows")) {
				str.erase(0, 6);
				frames = std::atoi(str.c_str());
			}
			else if (i == 12) {
				//Get the names of the muscles from header line 12
				std::string word = "";
				int m = 0;
				for (int k = 0; str[k] != '\0'; k++) {
					if (str[k] == ' ' || str[k] == '\t') {
						if (k >= 1) {
							if (str[k - 1] != ' ' && str[k - 1] != '\t') {
								Muscledata data;
								data.name= word;
								muscledata.push_back(data);
								word.erase();
								m++;
							}
						}
					}
					else word.append(str, k, 1);
				}
				Muscledata data;
				data.name = word;
				muscledata.push_back(data);
				

				
				}
			}
		*_muscledata = muscledata;
		*_frames = frames;
		}
	
	void MuscleDataInput::loadMuscleAllocation(char const* file, std::vector<Muscle_allocdata> *_ma_data) {
		if (file == NULL) throw CForgeExcept("No file specified!");

		std::ifstream inFile;
		inFile.open(file);

		std::string str;
		int str_length = str.length();
		int arr[] = { 0 };
		std::vector<Muscle_allocdata> ma_data;

		while (std::getline(inFile, str)) {

			Muscle_allocdata data;

			std::string word = "";
			int j = 0;
			
			for (int k = 0; str[k] != '\0'; k++) {
				if (str[k] == ' ' || str[k] == '\t') {
					if (k >= 1) {
						if (str[k - 1] != ' ' && str[k - 1] != '\t') {
							if (j == 0) data.name = word;
							else if (j == 1) data.min = std::atof(word.c_str());
							word.erase();
							j++;
						}
					}
				}
				else {
					word.append(str, k, 1);

				}
			}

			data.max = std::atof(word.c_str());
			ma_data.push_back(data);
		}

		*_ma_data = ma_data;
	}




}//name space