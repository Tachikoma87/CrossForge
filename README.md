# CrossForge: Your tool for cross-platform 3D-Apps
<p align="center">
    <img src="Documentation/CrossForgeLogoPackage/CrossForge-Logo-Banner.jpg" alt="CrossForge Logo Banner"/>
</p>

![C++](https://img.shields.io/badge/C++-00599C.svg?style=for-the-badge&logo=C++&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C.svg?style=for-the-badge&logo=CMake&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white) 
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![WebAssembly](https://img.shields.io/badge/WebAssembly-654FF0.svg?style=for-the-badge&logo=WebAssembly&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub release](https://img.shields.io/github/release/CrossForge/StrapDown.js.svg)](https://GitHub.com/CrossForge/StrapDown.js/releases/)


# Building CrossForge

## Building using emscripten

From top directory call
emcmake cmake -DWEBP_USE_THREAD=OFF -B out/build-em -Wno-dev
cmake --build out/build-em


How to cite:

    @misc{CForge,
        title={CrossForge: A Cross-Platform Visualization Library},
        author={Tom Uhlmann and Contributors},
        year = {2020-2023},
        howpublished = {Github (not yet publicly available)},
    }

# Acknowledgments

Logo-Background: <a href="https://www.freepik.com/free-vector/burst-effect-with-falling-fire-sparks_30800556.htm#query=blacksmith&position=39&from_view=search&track=sph">Image by upklyak</a> on Freepik



# Getting Started
 CrossForge is a cross platform graphics framework for 3D applications on Windows, Linux, and WebAssembly. It is primarily designed for educational purposes and computer graphics research. Hence it focuses solely on OpenGL as graphics API (no DirectX or Vulcan), and features extensive tutorials for computer graphics novices. If you want to learn how 3D application work from the ground up, you are at the right place.

# Why CrossForge?
- lot of very impressive open source graphic engines out there
- most of them tend to support various graphics APIs (Vulcan/DirectX/OpenGL) and abstract away the underlying API
- CrossForge purely focuses on OpenGL with strong focus on compatibility with WebGl, so publication for web pages
- this enables you to solely focus on a single graphics API and write your own code and shaders for it using popular and most  widespread graphics API
- develop desktop and web 3D apps alike with the same code
- perfect for academics in computer graphic
 

# List of features

 + Supported systems: Windows, Linux, and Websites (WebAssembly through Emscripten)
 + Physically based shading and material system
 + Deferred and forward rendering pipelines
 + Holistic self-configuring Shader
 + Default Actors for:
	+ Static models
	+ Skeletal animations
	+ Morph target animations
+ Shadow mapping
 

# Building the CrossForge Framework
 This section explains how to setup and build the CrossForge framwork.

 - install git
 - copy repository with git

## Windows
 - install visual studio
 - install vcpkg
 - open folder in visual studio, wait
 - compile and execute

## WSL
 - install Ubuntu
 - install vcsrv
 - install xfce4
 - configure visual studio
 - compile and execute

## Linux
 - install cmake
 - cmake commands

## Emscripten
 - install emscripten
 - emcmake commands
 - requires local webserver to view

# Next Steps
- follow our tutorials
