#pragma once

enum presetOption {
	MIN,
	LOW,
	MED,
	HIG,
	MAX,
	CUSTOM
};

presetOption loadSettings() {
	std::ifstream infile("Assets/qualitySelection.txt");
	std::string userSetting;
	std::getline(infile, userSetting);
	infile.close();
	std::cout << userSetting << "\n\n";

	if (userSetting == "min") return MIN;
	if (userSetting == "low") return LOW;
	if (userSetting == "med") return MED;
	if (userSetting == "hig") return HIG;
	if (userSetting == "max") return MAX;
	if (userSetting == "custom") return CUSTOM;
	std::cout << "False quality Option!";
	return MIN;
}

// select graphics setting preset or custom settings or load settings from disk
presetOption selectedPreset = loadSettings();

// adjust values for custom settings here:
struct settings {
	int windowWidth = 1920;// 5120 / 4;		// window width
	int windowHeight = 1080;// 1440 / 2;	// window height
	bool ssr = true;				// screen space reflection
	float ssrResolution = 0.75;		// has to be bigger than 0 and the lower the value the cheaper the SSR is.
	bool blur = false;				// gaussian blur on water reflections
	bool shoreWaves = true;		// enables or disables shore waves
	int dekorationDensity = 25;		// scale value for number of dekoration objects
	bool allowRocks = true;			// enables or disables rock placement in rivers
	float sizeScale = 6;			// has to be bigger than 0 and scales resolution of the terrain (bigger is more expensive for rendering and precomputation)
	int wSGridSize = 256;			// wave simulation grid size (bigger is more expensive, recommended to use power of between 64 and 512)

};


settings gSettings =	selectedPreset == MIN ? settings {	.windowWidth = 640, .windowHeight = 480,	.ssr = false,	.ssrResolution = 0.5,	.blur = false,	.shoreWaves = false,	.dekorationDensity = 0,		.allowRocks = false,	.sizeScale = 1,		.wSGridSize = 32	} :
						selectedPreset == LOW ? settings {	.windowWidth = 1280, .windowHeight = 720,	.ssr = false, 	.ssrResolution = 0.5,	.blur = false,	.shoreWaves = false,	.dekorationDensity = 10,	.allowRocks = false,	.sizeScale = 2,		.wSGridSize = 64	} :
						selectedPreset == MED ? settings {	.windowWidth = 1280, .windowHeight = 720,	.ssr = true,	.ssrResolution = 0.5,	.blur = false,	.shoreWaves = true,		.dekorationDensity = 15,	.allowRocks = true,		.sizeScale = 4,		.wSGridSize = 128	} :
						selectedPreset == HIG ? settings {	.windowWidth = 1920, .windowHeight = 1080,	.ssr = true,	.ssrResolution = 0.75,	.blur = false,	.shoreWaves = true,		.dekorationDensity = 25,	.allowRocks = true,		.sizeScale = 6,		.wSGridSize = 256	} :
						selectedPreset == MAX ? settings {	.windowWidth = 1920, .windowHeight = 1080,	.ssr = true,	.ssrResolution = 1.0,	.blur = false,	.shoreWaves = true,		.dekorationDensity = 75,	.allowRocks = true, 	.sizeScale = 8,		.wSGridSize = 512	} :
						settings();
	


