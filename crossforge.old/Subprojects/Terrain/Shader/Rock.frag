#version 330 core 

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

layout (std140) uniform MaterialData{
	vec4 Color;
	float Metallic;
	float Roughness;
	float AO;
	float Padding;
}Material;

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
// TexDepth (R|G|B|A): (Roughness|Ambient Occlusion|DepthMap|Metallic)
uniform sampler2D TexDepth;

in vec3 Pos;
in vec3 N;
in vec3 B;
in vec3 T;
in vec2 UV;
in vec3 CameraPos;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main(){
	// enable or disable different mapping options
	bool paraMapping = false;
	bool normMapping = false;
	bool ambiMapping = false;
	bool rougMapping = false;
	bool metaMapping = false;

	// scale down Texture
	const float uvScale = 7;
	vec2 smallUV = UV * uvScale;

	// tangent space Matrix
	mat3x3 tangentSpace = mat3x3(T, B, N);

	// parallaxMapping
	if (paraMapping) {
		vec3 viewDir = normalize(inverse(tangentSpace) * (CameraPos - Pos));
		smallUV = ParallaxMapping(smallUV, viewDir);
	// only for planes!
	//	if((smallUV.x / uvScale) > 1.0 || (smallUV.y / uvScale) > 1.0 || (smallUV.x / uvScale) < 0.0 || (smallUV.y / uvScale) < 0.0) {
	//		discard;
	//	}
	}
	
	// POSITION ---------------------------------------------------------------------------------
	gPosition = vec4(Pos, Material.AO);
	
	// NORMAL  ----------------------------------------------------------------------------------
	float r;
	if (rougMapping) {
		r = texture(TexDepth, smallUV).r;
	}
	else {
		r = 0;//Material.Roughness;
	}

	if (normMapping) {
		vec3 normalValue = texture(TexNormal, smallUV).rgb * 2 - 1;
		gNormal = vec4(normalize(tangentSpace * normalValue), r);
	}
	else {
		gNormal = vec4(normalize(N), r);
	}

	// COLOR ---------------------------------------------------------
	if (ambiMapping) {
		gAlbedoSpec.rgb = Material.Color.rgb * texture(TexAlbedo, smallUV).rgb * texture(TexDepth, smallUV).g;
	}
	else {
		gAlbedoSpec.rgb = Material.Color.rgb * texture(TexAlbedo, smallUV).rgb;
	}

	// SPECULAR ---------------------------------------------------------------------------------
	if (metaMapping) {
		gAlbedoSpec.a = texture(TexDepth, smallUV).a;
	}
	else {
		gAlbedoSpec.a = Material.Metallic;
	}
}


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) { 
	// lower number of layers = better performance
	const float maxLayers = 16;
	const float minLayers = 8;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));  

	// height scale
	const float intensity = 0.05;

	// calc step distance Vector for loop
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;
	vec2 p = viewDir.xy * intensity;
	vec2 deltaTexCoords = p / numLayers;
  
    // get initial values
	vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = texture(TexDepth, currentTexCoords).b;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(TexDepth, currentTexCoords).b;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(TexDepth, prevTexCoords).b - currentLayerDepth + layerDepth;
 
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;   
}   