
CrossForge: Your research and education tool for cross-platform 3D-Apps
========================

<!-- {#mainpage} Add this after the first section label to make Doxygen recognize this page as mainpage-->

<p align="center">
    <img src="Documentation/Images/CrossForge-Logo-Banner.webp" alt="CrossForge Logo Banner"/>
</p>

![C++](https://img.shields.io/badge/C++-00599C.svg?style=for-the-badge&logo=C++&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C.svg?style=for-the-badge&logo=CMake&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white) 
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![WebAssembly](https://img.shields.io/badge/WebAssembly-654FF0.svg?style=for-the-badge&logo=WebAssembly&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub release](https://img.shields.io/github/release/CrossForge/StrapDown.js.svg)](https://GitHub.com/CrossForge/StrapDown.js/releases/)

<p style="text-align: justify">
 CrossForge is a cross platform graphics framework for 3D applications on Windows, Linux, and WebAssembly. It is primarily designed for educational purposes and computer graphics research. Hence it focuses solely on OpenGL as graphics API (no DirectX or Vulcan), and features extensive tutorials for computer graphics novices. If you want to learn how 3D application work from the ground up, you are at the right place.
</P>

# Why CrossForge?
<img align="right" height="200" style="margin:0px 12px" src="Documentation/Images/Examples/Lighting.webp"/>
There are a lot of very impressive open source graphics engines out there and most of them support various graphics APIs like Vulcan, DirectX and OpenGL. This is only possible if the underlying API is abstracted away and you are forced to use the interface of the respective engine. CrossForge solely uses OpenGL with a strong focus on compatibility with WebGL in order to be compatible with WebAssembly. Hence, the publication of your CrossForge projects on a website is a built-in feature and not just an option. This design allows you to use the available CrossForge modules, modify them, or completely replace them with you own OpenGL based implementation and GLSL Shaders. This flat design and reduced complexity makes the CrossForge framework perfect for education and research in computer graphics as well as for hobbyists. 

# Feature List
+ Supported systems: Windows, Linux, and WebAssembly
+ OpenGL only
+ Physically based shading (PBS) and material system
+ Deferred and forward rendering pipelines
+ Holistic self-configuring Shader system
+ Shadow mapping
+ Scene graph system
+ Skeletal and morph-target animations
+ Text rendering

See the <a href="Examples/">examples</a> to get a better impression of what CrossForge is capable of.

# Building CrossForge
## Basic Setup
The following steps have to be performed, no matter what OS you are running.

1. If you haven't done already, install git <a href="https://git-scm.com/book/en/v2/Getting-Started-Installing-Git">https://git-scm.com/book/en/v2/Getting-Started-Installing-Git</a> and clone this repository with

        > git clone https://github.com/Tachikoma87/CrossForge.git

2. CrossForge relies on vcpkg as package manager on Windows and Linux <a href="https://github.com/microsoft/vcpkg">https://github.com/microsoft/vcpkg</a>. Follow the installation instructions for your system: <a href="https://github.com/microsoft/vcpkg#getting-started">https://github.com/microsoft/vcpkg#getting-started</a>

## Windows
The recommended tool for building CrossForge on Windows in Visual Studio. Download your preferred version from <a href="https://visualstudio.microsoft.com">https://visualstudio.microsoft.com</a> and make sure you select **C++CMake Tools for Windows** during installation. You can also add the CMake Tools using the Visual Studio Installer, if Visual Studio is already installed:

<p align="center"><img src="Documentation/Images/VisualStudioInstallInstructions.webp" alt="Visual Studio Install Instrutions"/> </p>

Now we need vcpkg to work with Visual Studio. Open a command prompt and type

    > [path-to-vcpkg]\vcpkg integrate install

Start Visual Studio and select **Open a local folder**. Move to the directory where you executed the first git command and open the **CrossForge** folder. If the CMake configuration process does not start automatically right click on **CMakeLists.txt** and select **configure cache**. Wait for the configuration to complete, then select **Build->Build All (F7)** from the menu bar. After completion select *CForgeSandbox.exe* as starting item and select **Debug->Start Debugging (F5)** from the menu bar or click on the small green right facing arrow.

<p align="center">
<img src="Documentation/Images/VSOpenFolderAndConfigureCache.webp" alt="Visual Studio Open Folder" height="400"/>
</p>

## Linux
CrossForge uses vcpkg in manifest mode, which means that dependencies should be installed automatically. Run the following commands from the top directory to build the software:

    > cmake -B out/build-lin -S . "-DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake"
    > cmake --build out/build-lin

And run the sandbox with

    > cd out/build-lin
    > ./CForgeSandbox

## Emscripten
Install emscripten by following <a href="https://emscripten.org/docs/getting_started/downloads.html">https://emscripten.org/docs/getting_started/downloads.html</a>. 
Open a command console and change directory to CrossForge folder.
From top directory run
    
    > [path-to-emsdk]/emsdk activate latest
    > emcmake cmake -DWEBP_USE_THREAD=OFF -B out/build-ems -Wno-dev  
    > cmake --build out/build-ems

To test the created website you will need a local webserver. You can create one on WSL or Linux following this tutorial: <a href="https://ubuntu.com/tutorials/install-and-configure-apache#1-overview">https://ubuntu.com/tutorials/install-and-configure-apache#1-overview</a>

 ## Windows Subsystem for Linux (WSL)
1. Prepare WSL and Visual Studio for a remote connection: <a href="https://learn.microsoft.com/en-us/cpp/linux/connect-to-your-remote-linux-computer?view=msvc-170">https://learn.microsoft.com/en-us/cpp/linux/connect-to-your-remote-linux-computer?view=msvc-170</a>

1. By default WSL does not come with graphics. You can configure your WLS with graphics and audio using this guide: <a href="https://research.wmz.ninja/articles/2017/11/setting-up-wsl-with-graphics-and-audio.html">https://research.wmz.ninja/articles/2017/11/setting-up-wsl-with-graphics-and-audio.html</a>. 

1. Then install and start xfce4 (or any other graphical desktop you like) in your WSL

        > sudo apt-get install xfce4
        > startxfce4

1. Open Visual Studio and the CrossForge project. 
1. In the CMake Settings **(Project->CMake Settings for ...)** add the Linux-GCC-Debug and/or Linux-GCC-Release as target. 
1. Choose the WSL connection as Remote machine name.
1. Switch to a Linux-GCC target and wait for the CMake configuration to finish
1. Build the project **(F7)**
1. Run the project **(F5)**. The window will open in the xfce4 desktop.


# How to cite:

    @misc{CForge,
        title={CrossForge: A Cross-Platform 3D Visualization and Animation Framework for Research and Education in Computer Graphics},
        author={Tom Uhlmann and Contributors},
        year = {2020-2023},
        howpublished = {https://github.com/Tachikoma87/CrossForge},
    }

# Acknowledgment
<table style="margin-left:auto;margin-right:auto">
    <tr>
        <td><img href="https://hybrid-societies.org/" src="Documentation/Images/Logo_HybridSocieties.webp" height="150p"/></td>
        <td><img href="https://www.tu-chemnitz.de/" src="Documentation/Images/LogoTUChemnitz.webp" height="150p" /></td>
    </tr>
</table>
This project was originally developed at Chemnitz University of Technology and funded by the Deutsche Forschungsgemeinschaft (DFG, German Research Foundation) – Project-ID 416228727 – CRC 1410.

# Credits
+ Logo-Background: <a href="https://www.freepik.com/free-vector/burst-effect-with-falling-fire-sparks_30800556.htm#query=blacksmith&position=39&from_view=search&track=sph">Image by upklyak</a> on Freepik
+ Default Fonts by Google Fonts <a href="https://fonts.google.com/">https://fonts.google.com/</a>
+ PRR materials from ambientCG: <a href="https://ambientcg.com/">https://ambientcg.com/</a>
+ 3D models from the Khronos gltf samples repository: <a href="https://github.com/KhronosGroup/glTF-Sample-Models">https://github.com/KhronosGroup/glTF-Sample-Models</a>



