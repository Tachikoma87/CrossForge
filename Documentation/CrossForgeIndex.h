
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
Iteration: Skeletal Animation
	* [x] Read article on Skeletal Animation
	* [x] Read skeletal animation data from file
	* [x] Extend T3DMesh with skeletal data
	* [x] Create and implement SkeletalActor
	* [x] adapt shader for skeletal animation
	* [x] make shadows work with skeletal animation
	* [ ] move skeletal actor and stuff to library
	
Iteration: Morph Target animation
	* [x] extend T3DMesh with Morph targets
	* [x] create Morph target builder class (takes topological identical meshes and creates morph targets)
	* [x] write shader for morph target animation
	* [x] implement UBO for morph target animation
	* [x] implement morph target animation controller (creates the animation data, holds UBO and morph target data [GLBuffer])
	* [x] implement MorphTargetActor
	* [ ] move Morph Target classes to core
	
Example Scenes:
	[ ] create Example assets folder and push to repo
	[ ] import dome from Digitale Objektrekonstruktion and update Minimum Graphical setup
	[ ] write example for lighting/shadow
	[ ] write example for skeletal animation
	[ ] write example for morph target animation

Miscellaneous Stuff:
	[x] add method to build transformation from node
	[ ] rethink concept in ShaderManager to not create same shader multiple times (maybe compare final code)

* Ideas for next iteration: 
	* implement mesh processing operations (halfe edge DS, IGL methods)
		* https://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
	* Normal Mapping, Parallax Mapping, 
	* Point Based Animation, 
	* post processing (Bloom)
	* Video capture (separate thread that writes images to HDD and then converts to video)
	* Geometry shader to visualize normal vectors, 
	* view frustum culling (scene graph),
	* WebAssembly support
*/


/***** Completed implementation parts

* [completed] Iteration: Model Visualization (Render geometry of a 3D model loaded from a file (obj/fbx)
	* [x] Setup AssetIO (Singleton) that loads models using plugin model
	* [x] Create T3DMesh as interchangeable format for 3d model data (hierarchical model, vertices, materials global but faces in hierarchy)
	* [x] Add Assimp as plugin to load 3d geometry and load geometry data of a model to T3DMesh
	* [x] Create StaticActor derived from IRenderable that sets up a TTriangleMesh for rendering (build buffers and stuff)
	* [x] Create Shader class that compiles, builds, and binds a shader and provides links to common binding points (vertex attributes, textures, and stuff)
	* [x] implement camera class (holds projection matrix, builds view matrix)
	* [x] implement render device, that holds rendering informations and actually renders something upon request
	* [x] render geometry of a model
	*
* [Completed] Iteration: Optimized control
	* [x]Implement uniform buffer objects and make locations accessible through shader
	* [x] Finish VirtualCamera and make moving around possible
	* [x] Implement ShaderManager (Singleton) that creates shaders from sources files

* [Completed] Iteration: Textured model (load textures and render textured model)
	* [x] Extend AssetIO to load textures (write plugins, use OpenCV at first)
	* [x] create TImage2D simple 2D image
	* [x]create OpenGL texture class that converts TImage2D to OpenGL texture
	* [x] extend material and shader to handle textures
	* [x] extend shader to use materials
	* [x] render textured model

* [Completed] Iteration: Lighting model (setup pBR based lighting and render model)
	* [x] Implement Material of a model (T3DMesh)
	* [x] extend Assimp plugin to handle materials
	* [x] extend shader and render device so it can handle materials
	* [x] extend StaticActor to handle materials
	* [x] write appropriate Shaders and render model
	* [x] implements lights and update lightsUBO/render device/shader
	* [x] implement PBS material data
	* [x] implement actor dynamic change of material data
	* implement computation of normals in T3DMesh
	*
* Iteration: Deferred Shading
	* [x] implement GBuffer
	* [x] implement Geometry pass
	* [x] Use GBuffer and outputs contents in image(Normal, Position)
	* [x] implement ScreenQuad Actor
	* [x] implement lighting pass
	* 
* [completed] Iteration: Shadows
	* [x] Setup a simple scene with floor and some cubes/spheres
	* [x] read article and implement shadow mapping | also did peter-panning, shadow map filtering, removing shadow acne
	* [x] add lights as part of RenderDevice (distinct between dir, spot and point lights)
	* implement GraphicsUtility class (
		* [x] perspective, orthographic projection matrix, look at matrix,
		* [x] checkGLError,
		* [x] read texture to T2Dimage,
		* [x] add deg to rad template function
		* [x] Screenshot function (save to image)
		* [x] read gpu memory
	*
* [completed] Iteration: Forward rendering pass
	* [x] add forward rendering pass
	* [x] blit render results correctly
	* [x] add image adjustment to shader
	*

* [Completed] Iteration: Callback system
	* [x] implement Caller and Listener templates and use to automatically update camera (test scenario)

* [completed] Iteration: Lights II
	* [x]Fully implement point and spot lights
	*
* [completed] Iteration: configurable super shader
	* [/] (postponed)implement a super shader that can be configured by various parameters/attributes (implemented in SShaderManager)
	* [x] add option to switch between blin phong and PBS
	* [x] implement majority of passes and settings in RenderDevice
	* 
* [completed] Iteration: Scene Graph (implement simple scene graph, maybe use concept/data from Hydra)
* [Completed] Iteration: Test Scene for Digitale Objektrekonstruktion
	* [x] create sample scene (Room, Table, 3 Objects [Head, Motorradantrieg, Bronzestatue, Bronzeskulptur])
	* [x] create default albedo texture (White) for materials without texture
	* [x] add enable option to SGNGeometry
	* [x] make Screenshot and video (for Guido Wochenbericht und Thomas)
* [completed] Iteration: Consolidation and Cross Platform
	* [x] try on Linux (works now on Raspberry)
	* [x] create CrossForgeBase class from that all objects are derived (they get assigned an ID, they register and unregister, they have a name)
	* [x] add documentation todo to all classes
	* [x] add export script (CMake copy stuff) - now a c++ script
	* [x] create separate project (Digitale Objektrekonstrutkion) that uses crossforgelib, write CMake export script, try separate project with library
	* [x] add method to configure shader (Version code and change constants)
		* [x] add option to shader building that identifies string to be shader code or URL (first character must be a sharp) (identification if provided "code" is a valid path)
		* [x] implement this in shader utility (now ShaderCode class that handles the actual code and provides options for configuration)
	* [x] write keyboard and mouse class (tracks keystrokes and has callback system, same for mouse)
	* [x] add mouse and keyboard and SInputManager classes to core and add to GLWindow
*/