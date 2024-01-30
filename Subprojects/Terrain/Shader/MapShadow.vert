#version 330 core 

//layout (location = 0) in vec3 Position;

uniform uint ActiveLightID;

const uint DirLightCount = 2U;

layout(std140) uniform DirectionalLightsData{
	vec4 Directions[DirLightCount];
	vec4 Colors[DirLightCount]; ///< a component is intensity
	mat4 LightSpaceMatrices[DirLightCount];
	ivec4 ShadowIDs[DirLightCount];
}DirLights;

#define MAX_INSTANCE_COUNT 500
layout(std140) uniform InstancedData{
	mat4 instancedMat[MAX_INSTANCE_COUNT];
};

in vec2 VertPosition;

uniform float MapHeight;
uniform sampler2D HeightMap;

void main(){
	//vec4 Po = vec4(Position, 1.0);
	//gl_Position = DirLights.LightSpaceMatrices[ActiveLightID] * instancedMat[gl_InstanceID] * Po;
	//return;
	
	mat4 ModelMatrix = instancedMat[gl_InstanceID];
	vec4 worldPosition = ModelMatrix * vec4(VertPosition.x, 0.0, VertPosition.y, 1.0);
	vec2 mapSize = textureSize(HeightMap, 0);
	
	// first 0.5 centeres the texel position and the second centers the map texture
	vec2 SamplePosition = (worldPosition.xz + 0.5) / mapSize + 0.5;
	float Height = texelFetch(HeightMap, ivec2(SamplePosition * mapSize), 0).x;
	
	worldPosition.y = Height * MapHeight;

	gl_Position = DirLights.LightSpaceMatrices[ActiveLightID] * worldPosition;
	//gl_Position = DirLights.LightSpaceMatrices[ActiveLightID] * instancedMat[gl_InstanceID] * Po;
}//main 
