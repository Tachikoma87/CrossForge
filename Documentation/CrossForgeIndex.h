
/** \mainpage CrossForge Documentation
*
* \section Introduction
*
*
* \section To-Do List
*
* \todo Include TinyXML
* \todo Add class for hardware modules: Stepper Driver, C1101
*
*
* Group definition
* \defgroup Core Members of the Cross Forge Core. Also includes various input/output and communication capabilities.
* \defgroup GUI Graphical user interface classes.
* \defgroup Graphics Graphical capabilities using OpenGL.
* \defgroup Devices Classes to use special hardware devices (e.g. sensors, actuators, etc.)
*
*/

/***
* Planning:	

	
Example Scenes:


Miscellaneous Stuff:
	[ ] rethink concept in ShaderManager to not create same shader multiple times (maybe compare final code)

* Ideas for next iteration: 
	* implement mesh processing operations (halfe edge DS, IGL methods)
		* https://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
	* Normal Mapping, Displacement Mapping (Also in LearnOpenGL Website)
	* Point Based Animation, 
	* Add Compute Shaders
	* post processing (Bloom)
	* Video capture (separate thread that writes images to HDD and then converts to video)
	* Geometry shader to visualize normal vectors, 
	* view frustum culling (scene graph),
	* WebAssembly support
	* Implement Instanced Rendering (Instanced Actor?)
	* Cascaded Shadow Mapping: https://learnopengl.com/Guest-Articles/2021/CSM
	* implement Text Rendering
	* Implement (or use) GUI
*/


/***** Completed implementation parts ****/