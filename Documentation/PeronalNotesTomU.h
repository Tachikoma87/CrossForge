/***
* Planning:
* 
Iteration: Skeletal Animation
	* Read article on Skeletal Animation
	* [x] Read skeletal animation data from file
	* [x] Extend T3DMesh with skeletal data
	* [x] Create and implement SkeletalActor
	* [x] adapt shader for skeletal animation
	* [x] make shadows work with skeletal animation
	* move skeletal actor and stuff to library
	* rethink concept in ShaderManager to not create same shader multiple times (maybe compare final code)

Iteration: Morph Target animation
	* [x] extend T3DMesh with Morph targets
	* create Morph target builder class (takes topological identical meshes and creates morph targets)
	* write shader for morph target animation
	* implement UBO for morph target animation
	* implement morph target animation controller (creates the animation data, holds UBO and morph target data [GLBuffer])
	* implement MorphTargetActor
	*
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