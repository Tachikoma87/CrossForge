
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
*
* \defgroup Core Members of the Cross Forge Core. Also includes various input/output and communication capabilities.
*
* \defgroup GUI Graphical user interface classes.
*
* \brief These classes form the basis of a simple GUI implementation.
*
* For most simple use cases a developer will probably only need to
* interact the the main class, CForge::GUI, and the few widgets,
* it will return to you
*
* Generally, in order to use the GUI, you should always create an
* instance of CForge::GUI first. Then you can use its methods to
* create widgets, or create some of your own. During the program's
* render loop, two methods should be queried each frame:
* CForge::GUI::processEvents and CForge::GUI::render. That is to
* make the GUI aware of user input and to draw it onto the framebuffer
* respectively. Once the program terminates, clean up should be
* handled by CForge::GUI's destructor automatically, widgets
* generally don't need to be destroyed manually if they were in
* use within the GUI.
*
* In order to best faciliate callbacks into the program code, it's
* suggested to structure your program as class and have it inherit
* the CForge::ITListener<GUICallbackObject> interface. Then you
* can have everything that needs to be accessed by the callback
* available as member variables. However, other means of providing
* the callback function can also be used if deemed appropriate.
* For more information on this kind of callback, see the documentation
* of CForge::GUICallbackObject and CForge::FormWidget.
*
* \defgroup Graphics Graphical capabilities using OpenGL.
*
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
