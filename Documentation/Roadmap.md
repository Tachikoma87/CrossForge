# Roadmap

+ Version v0.X.0
	+ [ ] Integration of Bullet physics engine
	+ [ ] Displacement mapping
	+ [ ] Post processing (bloom)
	+ [ ] Integration of the polygon mesh processing library: https://www.pmp-library.org/
	+ [ ] Global simulation class (holds stats about the current simulation stats such as time)
+ Version v0.2.0
	+ Mesh processing
		+ [ ] integration of polygon mesh processing library (PMP)
		+ [ ] Mesh reduction
		+ [ ] Subdivision surfaces
	+ Miscellaneous
		+ [ ] Audio support
	+ Graphics
		+ [ ] Instanced rendering
		+ [ ] Cascaded shadow maps https://learnopengl.com/Guest-Articles/2021/CSM
		+ [ ] Point shadows	
		+ [ ] Render to texture
		+ [ ] Billboard actor
		+ [ ] LOD system
	+ Basic GUI System
		+ [ ] Basic widgets (Button, Sliders, Label, etc.)
		+ [ ] Basic Menu bar
	+ Examples
		+ [ ] Graphical User Interface
		+ [ ] Audio
+ Version v0.1.0
	+ General:
		+ [x] Support for Windows, Linux, and WebAssembly (Emscripten)
		+ [ ] Full in-code documentation
		+ [ ] General Documentation and tutorials
		+ [x] Math support class
	+ Graphics:
		+ [x] Basic framework to visualize 3D models (vertex buffer, index buffer, uniform buffer, etc.)
		+ [x] Loading of assets (3DModels and textures)
		+ [x] Deferred (GBuffer) and forward rendering
		+ [x] Physically based shading and PBS material system
		+ [x] Normal mapping
		+ [x] Separate visualizatin of the skeleton of a character
		+ [x] Directional, point, and spot lights
		+ [x[ Holistic self-configuring Shaders
		+ [x] Basic Skeletal animation
		+ [x] Basic morph target animations
		+ [x] Basic scene graph
		+ [x] Image post processing of scene
		+ [x] Multiple viewports
		+ [x] Skybox
		+ [x] Procedural generation of geometric primitives (Cube, Plane, Sphere, etc.)
		+ [x] Factory for simple materials (metals, stones, plastic)
		+ [x] View frustum culling
		+ [x] Basic shadows (shadow mapping)
		+ [x] Rendering of text
	+ Example scenes
		+ [x] Basic Scene setup
		+ [x] Minimum graphics setup
		+ [x] Multiple viewports
		+ [x] Scene graph
		+ [x] Skeletal animations
		+ [x} Morph target animations
		+ [x] Skybox
		+ [x] Sockets (TPC + UDP)
		+ [x] Primitive shape factory
		+ [x] Material factory
		+ [x] Text rendering
		+ [x] Lights
	+ Miscellaneous
		+ [x] TCP and UDP classes
		+ [x] Support for simple electronic devices (on Raspberry Pi)

*/