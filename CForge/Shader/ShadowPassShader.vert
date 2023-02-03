#version 330 core 

#ifdef SKELETAL_ANIMATION
const uint BoneCount = 19U;

layout (std140) uniform BoneData{
	mat4 SkinningMatrix[BoneCount];
}Bones;
#endif

layout (location = 0) in vec3 Position;
#ifdef SKELETAL_ANIMATION
layout (location = 4) in ivec4 BoneIndices;
layout (location = 5) in vec4 BoneWeights;
#endif

uniform uint ActiveLightID;

const uint DirLightCount = 2U;

layout(std140) uniform DirectionalLightsData{
	vec4 Directions[DirLightCount];
	vec4 Colors[DirLightCount]; ///< a component is intensity
	mat4 LightSpaceMatrices[DirLightCount];
	ivec4 ShadowIDs[DirLightCount];
}DirLights;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
	mat4x3 NormalMatrix;
};//ModelData

void main(){
	vec4 Po = vec4(Position, 1.0);

#ifdef SKELETAL_ANIMATION 
	mat4 T = mat4(0);
	for(uint i = 0U; i < 4U; ++i){
		T += BoneWeights[i] * Bones.SkinningMatrix[BoneIndices[i]];	
	}//for[4 weights]

	Po = T * Po;
#endif 

	gl_Position = DirLights.LightSpaceMatrices[ActiveLightID] * ModelMatrix * Po;
}//main 
