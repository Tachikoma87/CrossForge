#version 430 core 

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gReflection;

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

uniform sampler2D foamTexture;
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D foamBlendTexture;
uniform sampler2D heightMapTexture;
uniform sampler2D shoreDistTexture;
uniform sampler2D worldPosTexture;
uniform sampler2D waterCausticsTexture;
uniform sampler2D normalsTexture;
uniform sampler2D displacementTexture;

uniform sampler2D skyBackTexture;
uniform sampler2D skyBotTexture;
uniform sampler2D skyFrontTexture;
uniform sampler2D skyLeftTexture;
uniform sampler2D skyRightTexture;
uniform sampler2D skyTopTexture;

uniform float amplitudeScale;
uniform float choppyness;
uniform float widthScale;
uniform vec2 nearFarPlane;
uniform float uvScale;
uniform float time;
uniform vec2 windDirection;

uniform bool doSSR;
uniform float ssrRes;
uniform bool doShoreWave;
uniform float sSizeScale;

in vec2 UVcord;
in vec3 POS;
in vec3 CAM;
in float newAmplitudeScale;

vec4 getSkyboxColor(vec3 dir) {
	vec3 absDir = abs(dir);

	if (absDir.x > absDir.y && absDir.x > absDir.z) {
		if (dir.x > 0) return texture(skyRightTexture, (dir.zy / absDir.x * vec2(-0.5) + vec2(0.5)));
		else return texture(skyLeftTexture, (dir.zy / absDir.x * vec2(0.5, -0.5) + vec2(0.5)));
	}
	else if (absDir.y > absDir.x && absDir.y > absDir.z) {
		if (dir.y > 0) return texture(skyTopTexture, (dir.xz / absDir.y) * vec2(0.5) + vec2(0.5));
		else return texture(skyBotTexture, (dir.xz / absDir.y) * vec2(0.5, -0.5) + vec2(0.5));
	}
	else if (absDir.z > absDir.x && absDir.z > absDir.y) {
		if (dir.z > 0) return texture(skyBackTexture, (dir.xy / absDir.z * vec2(0.5, -0.5) + vec2(0.5)));
		else return texture(skyFrontTexture, (dir.xy / absDir.z * vec2(-0.5) + vec2(0.5)));
	}
	
	return vec4(1, 1, 0, 1);
}

vec4 reflectionColor(vec3 normal, vec3 SCREENUV) {
	vec4 baseSkyColor = vec4(47, 78, 108, 255) / 255;
	float maxDistance = 300;
	float steps = 100.0 * ssrRes;
	float resolution = maxDistance / steps;
	
	vec2 tSize = textureSize(colorTexture, 0);
	vec3 reflectDirection = normalize(reflect(normalize(POS - CAM), normal));
	if (reflectDirection.y < 0) return baseSkyColor * 2;

	if (!doSSR) return getSkyboxColor(reflectDirection);

	vec4 end = vec4(POS + reflectDirection * maxDistance, 1);
	vec3 posIncrement = (end.xyz - POS) / steps;
	
	vec2 uv;
	float depth;
	float sampleDepth;
	vec3 middlePos = POS;
	bool hit = false;

	for(int i = 0; i < steps; ++i) {
		middlePos += posIncrement / ssrRes;
		vec4 mid = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(middlePos, 1);
		uv = mid.xy / mid.w / 2 + 0.5;
		posIncrement *= 1.05;

		if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0 ) {
			return getSkyboxColor(reflectDirection) * 2;
		}

		depth = distance(middlePos, CAM);
		
		vec3 wPos = texture(worldPosTexture, uv).xyz;
		sampleDepth = distance(CAM, wPos);
		
		if (sampleDepth < depth && sampleDepth + 200 > depth && !(wPos == vec3(0)) && sampleDepth > distance(POS, CAM)) {
			hit = true;
			break;
		}
	}

	

	if (!hit) {
			return getSkyboxColor(reflectDirection) * 2;
	}
	else {
		float scaleFactor = 1 - pow(length((uv - 0.5) * 2), 8);

		float visibility = 1 - max(dot(normalize(CAM - POS), reflectDirection), 0);
		if (scaleFactor > 0) {
			vec4 ret = mix(getSkyboxColor(reflectDirection), texture(colorTexture, uv), visibility * scaleFactor);// texture(colorTexture, uv), visibility);
			ret.w = scaleFactor;
			return ret;
		}
	}

	return getSkyboxColor(reflectDirection)  * 2;
	
}

/*
vec4 reflectionColor(vec3 normal, vec3 SCREENUV) {
	float maxDistance = distance(CAM, POS);
	float resolution = lowQuality ? 8 : 4;
	int steps = 10;
	float thickness = 0.5;
	vec2 tSize = textureSize(colorTexture, 0);

	vec3 reflectDirection = normalize(reflect(normalize(POS - CAM), normal));


	//reflectDirection.y /= 2;
	if (reflectDirection.y < 0) {
		return getSkyboxColor(reflectDirection);
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
	float sampleDepth;
	

	//return vec4(sampleDepth > startDepth ? 1 : 0);

	bool hit = false;

	for(int i = 0; i < steps; ++i) {
		
		depth += depthIncrement;
		uv += increment;

		if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0 ) {
			return getSkyboxColor(reflectDirection);
		}

		
		//float sampleDepth = distance(texture(posTexture, uv).xyz, CAM);
		sampleDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, uv).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
		
		//return vec4(sampleDepth);
		 


		if (sampleDepth < depth && sampleDepth + 0.01 > depth) {
			hit = (sampleDepth < 1);
			break;
		}
	}

	

	if (!hit) {
			return getSkyboxColor(reflectDirection);
	}
	else if (sampleDepth > startDepth) {
		float scaleFactor = 1;// - pow(length((uv - 0.5) * 2), 8);

		float visibility = 1 - max(dot(normalize(CAM - POS), reflectDirection), 0);
		if (scaleFactor > 0) {
			vec4 ret = mix(getSkyboxColor(reflectDirection), sampleBluredColorBuffer(uv), visibility);// texture(colorTexture, uv), visibility);
			ret = ret * scaleFactor;
			ret.w = scaleFactor;
			return ret;
		}
	}

	return getSkyboxColor(reflectDirection);
	
}
*/

float getShoreWaveFactor() {
	float maxHeight = 0.5;
	float minHeight = 0.3;

	vec2 tSize = textureSize(heightMapTexture, 0);
	vec2 uvSampleCord = POS.xz;
	uvSampleCord += tSize / 2;
	uvSampleCord = uvSampleCord / tSize;
	uvSampleCord = clamp(uvSampleCord, 0, 1);

	
	float ret = 1 - texture(shoreDistTexture, uvSampleCord).x;


	float ret2 = texture(heightMapTexture, uvSampleCord).x;
	float dist = length(POS.xz / tSize);
	ret2 = (ret2 - minHeight) / (maxHeight - minHeight);
	ret2 -= clamp((dist * 2 - 0.8) * 3, 0, 1);
	clamp(ret2, 0, 1);

	return mix(ret, ret2, 0.75);
}

float shoreWave(float factor) {
	float waveLength = 1.0;
	float waveSpeed = 0.1;

	float foamBuildUp = waveLength * 0.3;
	float foamBuildDown = waveLength * 0.1;

	float a = mod(-time * waveSpeed + factor, waveLength) / foamBuildUp - waveLength / foamBuildUp + 1;
	a = clamp(a, 0, 1);

	float b = -mod(-time * waveSpeed + factor, waveLength) / foamBuildDown + 1;
	b = clamp(b, 0, 1);
	
	return clamp((a + b) * factor / 4, 0, 1);
}

vec3 getFoamColor(float foamFactor) {
	float foamUVScale = 50;
	vec3 foamScale = texture(foamBlendTexture, vec2(foamFactor, 0)).rgb;

	vec2 foamUV = POS.xz / foamUVScale;
	foamScale /= 2;
	vec3 temp = texture(foamTexture, foamUV).rgb;

	temp = temp * foamScale;

	return vec3(temp.r + temp.g + temp.b);
}

vec4 getWaterCausics(vec2 backGroundUV, float depthColorScale) {
	float speed = 0.02 * sqrt(sSizeScale);
	float uvScale = 0.03;
	vec2 wPos = texture(worldPosTexture, backGroundUV).xz;
	vec2 causticsUV1 = wPos * uvScale + vec2(time * speed / 4) + normalize(wPos) * speed * time;
	vec2 causticsUV2 = wPos * uvScale + vec2(-time * speed / 4) + normalize(wPos) * speed * time;
	return mix(texture(waterCausticsTexture, causticsUV1), texture(waterCausticsTexture, causticsUV2), 0.5) * clamp((3 - depthColorScale * 4), 0, 1);
}


void main(){
	vec3 screenUV = vec3(gl_FragCoord.xy / textureSize(colorTexture, 0), gl_FragCoord.z);
	vec3 N;
	vec2 UV = UVcord;

	vec4 derivatives = texture(normalsTexture, UV);// sampleNorm(UV);
	N = normalize(vec3((derivatives.x / (1 + derivatives.z)), uvScale / newAmplitudeScale * 0.03, (derivatives.y / (1 + derivatives.w))));

	//vec3 sunPos  = vec3(-1000000000, 1000000000, 1000000000);
	vec3 sunDir =	normalize(vec3(-0.6, 0.5, 1));	//normalize(sunPos - POS);
	vec3 viewDir = normalize(Camera.Position.xyz - POS);
	vec3 halfwayDir = normalize(sunDir + viewDir);

	float shininess = 1000;

	
	float R = dot(N, viewDir);

	vec4 baseOceanBlue = vec4(9, 12, 17, 255) / 255;//vec4(17, 28, 50, 55) / 255;;
	vec4 baseSkyColor = vec4(47, 78, 108, 255) / 255;
	float foamFactor = clamp(3 * clamp(0.97 + -1 * texture(displacementTexture, UV).w, 0, 1), 0, 1);

	float spec = pow(max(dot(N, halfwayDir), 0.0), shininess) * 3;
	float l = max(acos(dot(N, viewDir)) - 80, 0.0);

	
	float depthBackground = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, screenUV.xy).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthWater = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (screenUV.z * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthColorScale = 1 - clamp((depthBackground - depthWater) * nearFarPlane.y * 0.2, 0, 1);


	vec2 backGroundUV = clamp(screenUV.xy + derivatives.xy / 10.0 * depthColorScale, 0, 1);
	vec4 backgroundColor = vec4(texture(colorTexture, backGroundUV));
	vec4 causticsColor = getWaterCausics(backGroundUV, depthColorScale);
	backgroundColor += causticsColor;


	vec4 reflectColor = reflectionColor(N, screenUV);
	float reflectiveness = 0.3;
	reflectColor = mix(baseSkyColor, reflectColor, reflectiveness);
	R = clamp(pow(R, 0.5),0, 1);
	// COLOR ---------------------------------------------------------
	
	//gColor = mix(mix(baseSkyColor, reflectColor, reflectColor.w) + spec, mix(baseOceanBlue, backgroundColor, depthColorScale), R);
	gColor = mix(vec4(0) + spec, mix(baseOceanBlue, backgroundColor, depthColorScale), R);
	
	gColor += vec4(getFoamColor(shoreWave(getShoreWaveFactor() * 2)), 1) * (doShoreWave ? 1 : 0);

	//gAlbedoSpec = texture(worldPosTexture, screenUV.xy) / 500;
	//gAlbedoSpec = vec4(getShoreWaveFactor() * 2);

	gColor = vec4(gColor.rgb, R);
	gReflection = reflectColor;
}
