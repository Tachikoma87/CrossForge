#version 430 core 

out vec4 gAlbedoSpec;

layout(rgba32f, binding = 0) uniform image2D textureDisplacement;
layout(rgba32f, binding = 1) uniform image2D textureNormal;

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

uniform sampler2D foamTexture;
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D posTexture;

uniform float amplitudeScale;
uniform float choppyness;
uniform float widthScale;
uniform vec2 nearFarPlane;

in vec2 UV;
in vec3 POS;
in vec3 CAM;
in vec3 SCREENUV;
in float newAmplitudeScale;

vec4 reflectionColor(vec3 normal) {
	float maxDistance = distance(CAM, POS);
	float resolution = 3;
	int steps = 10;
	float thickness = 0.5;
	vec2 tSize = textureSize(colorTexture, 0);

	vec3 reflectDirection = normalize(reflect(normalize(POS - CAM), normal));
	//reflectDirection.y /= 2;
	if (reflectDirection.y < 0) {
		return vec4(0);
	}
	vec4 end = vec4(POS + reflectDirection * maxDistance, 1);
	vec4 endPos = Camera.ProjectionMatrix * Camera.ViewMatrix * end;

	vec2 startUV = SCREENUV.xy;
	vec2 endUV = endPos.xy / endPos.w / 2 + 0.5;
	float startDepth = SCREENUV.z;
	float endDepth = endPos.z / endPos.w / 2 + 0.5;
	startDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (startDepth * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	endDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (endDepth * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	vec2 startFrag = startUV * tSize;
	vec2 endFrag = endUV * tSize;
	vec2 delta = endFrag - startFrag;
	steps = int(length(delta) / resolution);
	
	vec2 increment = (endUV - startUV) / steps;
	float depthIncrement = (endDepth - startDepth) / steps;
	



	vec2 frag = startFrag;
	vec2 uv = startUV;
	float depth = startDepth;
	
	

	//return vec4(sampleDepth > startDepth ? 1 : 0);

	bool hit = false;

	for(int i = 0; i < steps; ++i) {
		
		depth += depthIncrement;
		uv += increment;

		if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0 ) {
			return vec4(0, 0, 0, 0);
		}

		
		//float sampleDepth = distance(texture(posTexture, uv).xyz, CAM);
		float sampleDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, uv).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
		
		//return vec4(sampleDepth);
		 


		if (sampleDepth < depth && sampleDepth + 0.01 > depth) {
			hit = (sampleDepth < 1);
			break;
		}
	}

	

	if (!hit) {
		
		vec4 noHitPos = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(POS + reflectDirection * 500, 1);
		vec2 noHitUV = noHitPos.xy / noHitPos.w / 2 + 0.5;
		float scaleFactor = 1 - pow(length((noHitUV - 0.5) * 2), 8);
		if (scaleFactor > 0) {
			vec4 ret = texture(colorTexture, noHitUV) * scaleFactor;
			ret.w = scaleFactor;
			return ret;
		}
	}
	else {
		float scaleFactor = 1 - pow(length((uv - 0.5) * 2), 8);

		float visibility = 1 - max(dot(normalize(CAM - POS), reflectDirection), 0);
		if (scaleFactor > 0) {
			vec4 ret = texture(colorTexture, uv) * visibility;
			ret = ret * scaleFactor;
			ret.w = scaleFactor;
		return ret;
		}
	}

	return vec4(0);
	

	/*
	vec3 positionFrom = POS - CAM;
	vec3 unitPositionFrom = normalize(positionFrom);
	vec3 pivot = normalize(reflect(unitPositionFrom, normal));

	vec4 startView = vec4(positionFrom + (pivot * 0), 1);
	vec4 endView = vec4(positionFrom + (pivot * maxDistance), 1);

	vec4 startFrag = startView;
	startFrag = Camera.ProjectionMatrix * startFrag;
	startFrag.xyz /= startFrag.w;
	startFrag.xy = startFrag.xy / 2.0 + 0.5;
	startFrag.xy *= tSize;

	vec4 endFrag = endView;
	endFrag = Camera.ProjectionMatrix * endFrag;
	endFrag.xyz /= endFrag.w;
	endFrag.xy = endFrag.xy / 2.0 + 0.5;
	endFrag.xy *= tSize;

	vec2 frag = startFrag.xy;
	vec2 uv = frag / tSize;
	float deltaX = endFrag.x - startFrag.x;
	float deltaY = endFrag.y - startFrag.y;

	float useX = abs(deltaX) >= abs(deltaY) ? 1 : 0;
	float delta = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0, 1);

	vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.001);

	float search0 = 0;
	float search1 = 0;
	int hit0 = 0;
	int hit1 = 0;

	float viewDistance = startView.z;
	float depth = thickness;

	float positionTo;

	for (int i = 0; i < int(delta); ++i) {
		frag += increment;
		uv.xy = frag / tSize;
		positionTo = texture(depthTexture, uv).r;
		search1 = mix((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);
		viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);

		depth = viewDistance - positionTo;

		if (depth > 0 && depth < thickness) {
			hit0 = 1;
			break;
		}
		else {
			search0 = search1;
		}

		search1 = search0 + ((search1 - search0) / 2);
	}

	/*
	steps *= hit0;
	for (int i = 0; i < steps; i++) {
		frag = mix(startFrag.xy, endFrag.xy, search1);
		uv.xy = frag / tSize;
		positionTo = texture(depthTexture, uv).r;

		viewDistance = (startView.z)
	}
	*/
	
	//return texture(colorTexture, uv);
}

vec4 sampleDisp(vec2 uv) {
    ivec2 samplePos = ivec2(uv * imageSize(textureDisplacement));
    
    vec2 t = uv * imageSize(textureDisplacement) - ivec2(uv * imageSize(textureDisplacement));
    vec4 c00 = imageLoad(textureDisplacement, ivec2(mod(samplePos, imageSize(textureDisplacement))));
    vec4 c10 = imageLoad(textureDisplacement, ivec2(mod(samplePos + ivec2(1, 0), imageSize(textureDisplacement))));
    vec4 c01 = imageLoad(textureDisplacement, ivec2(mod(samplePos + ivec2(0, 1), imageSize(textureDisplacement))));
    vec4 c11 = imageLoad(textureDisplacement, ivec2(mod(samplePos + ivec2(1, 1), imageSize(textureDisplacement))));

    return (1 - t.x) * (1 - t.y) * c00 +
            t.x * (1 - t.y) * c10 +
            (1 - t.x) * t.y * c01 +
            t.x * t.y * c11;
}

vec4 sampleNorm(vec2 uv) {
    ivec2 samplePos = ivec2(uv * imageSize(textureNormal));
    
    vec2 t = uv * imageSize(textureNormal) - ivec2(uv * imageSize(textureNormal));
    vec4 c00 = imageLoad(textureNormal, ivec2(mod(samplePos, imageSize(textureNormal))));
    vec4 c10 = imageLoad(textureNormal, ivec2(mod(samplePos + ivec2(1, 0), imageSize(textureNormal))));
    vec4 c01 = imageLoad(textureNormal, ivec2(mod(samplePos + ivec2(0, 1), imageSize(textureNormal))));
    vec4 c11 = imageLoad(textureNormal, ivec2(mod(samplePos + ivec2(1, 1), imageSize(textureNormal))));

    return (1 - t.x) * (1 - t.y) * c00 +
            t.x * (1 - t.y) * c10 +
            (1 - t.x) * t.y * c01 +
            t.x * t.y * c11;
}

void main(){
	vec3 N;

	/*
	float a = imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement) - ivec2(0, 1.0))).y;
    float b = imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement) + ivec2(0, 1.0))).y;
    float c = imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement) - ivec2(1.0, 0))).y;
    float d = imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement) + ivec2(1.0, 0))).y;
	*/

	float a = sampleDisp(UV - vec2(0, 1.0) / imageSize(textureDisplacement)).y;
    float b = sampleDisp(UV + vec2(0, 1.0) / imageSize(textureDisplacement)).y;
    float c = sampleDisp(UV - vec2(1.0, 0) / imageSize(textureDisplacement)).y;
    float d = sampleDisp(UV + vec2(1.0, 0) / imageSize(textureDisplacement)).y;


    //N = normalize(vec3(a - b, widthScale / amplitudeScale, c - d)); 

	vec4 derivatives = sampleNorm(UV);
	N = normalize(vec3((derivatives.x / (1 + derivatives.z)), (min(1, length(CAM - POS) / 30.0)) * widthScale / newAmplitudeScale, (derivatives.y / (1 + derivatives.w))));

	vec3 sunPos  = vec3(-100000000, 1000000000, 1000000000);
	vec3 sunDir = normalize(sunPos - POS);
	vec3 viewDir = normalize(Camera.Position.xyz - POS);
	vec3 halfwayDir = normalize(sunDir + viewDir);

	float shininess = 256;

	float rIndexAir = 1.000293;
	float rIndexWater = 1.333;

	float r0 = pow((rIndexAir - rIndexWater) / (rIndexAir + rIndexWater), 2);
	float R = r0 + (1 - r0) * pow(1 - cos(dot(N, viewDir)), 1);

	vec4 baseOceanBlue = vec4(17, 28, 50, 55) / 555;
	vec4 baseSkyColor = vec4(17*3, 28*3, 50*3, 55) / 455;//vec4(182, 216, 228, 255) / 255;
	float foamFactor = 3 * clamp(0.97 + -1 * sampleDisp(UV).w, 0, 1);

	float spec = pow(max(dot(N, halfwayDir), 0.0), shininess);
	float l = max(acos(dot(N, viewDir)) - 80, 0.0);

	vec4 backgroundColor = vec4(texture(colorTexture, SCREENUV.xy + derivatives.xy / 10.0));

	float depthBackground = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, SCREENUV.xy).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthWater = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (SCREENUV.z * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthColorScale = 1 - clamp((depthBackground - depthWater) * 50, 0, 1);
	vec4 reflectColor = reflectionColor(N);

	// COLOR ---------------------------------------------------------
	//gAlbedoSpec = baseBlue + schaum;
	//gAlbedoSpec = vec4(N,1);
	gAlbedoSpec = baseOceanBlue * (R) + baseSkyColor * (1 - R);
	
	gAlbedoSpec = gAlbedoSpec * (1 - reflectColor.w) + reflectColor;
	gAlbedoSpec = gAlbedoSpec * (1 - depthColorScale) + backgroundColor * depthColorScale;
	gAlbedoSpec = gAlbedoSpec + spec + texture(foamTexture, UV * 10) * foamFactor;
	
	//gAlbedoSpec = vec4(foamFactor);
	//gAlbedoSpec = vec4(depthColorScale);
	//gAlbedoSpec = reflectionColor(N);
	//gAlbedoSpec = vec4(CAM, 1);

	
	//gAlbedoSpec = reflectColor;
	
	
	
	//gAlbedoSpec = texture(posTexture, SCREENUV.xy);
	//gAlbedoSpec = vec4(depthBackground);

	
	
	
	gAlbedoSpec = vec4(gAlbedoSpec.rgb, 1);
	//gAlbedoSpec = vec4(UV, 0, 1);
	//gAlbedoSpec = texture(textureDepth, UV);
	

}
