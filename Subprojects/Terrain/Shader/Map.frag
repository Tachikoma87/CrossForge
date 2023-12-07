#version 330 core

// https://www.iquilezles.org/www/articles/texturerepetition/texturerepetition.htm

vec4 hash4( vec2 p ) { return fract(sin(vec4(1.0+dot(p,vec2(37.0,17.0)), 2.0+dot(p,vec2(11.0,47.0)), 3.0+dot(p,vec2(41.0,29.0)), 4.0+dot(p,vec2(23.0,31.0))))*103.0); }

vec4 textureNoTile(sampler2DArray samp, in vec2 uv, int index) {
    ivec2 iuv = ivec2( floor( uv ) );
    vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );

    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );

    // uv's, and derivatives (for correct mipmapping)
    vec3 uva = vec3(uv*ofa.zw + ofa.xy, index); vec2 ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec3 uvb = vec3(uv*ofb.zw + ofb.xy, index); vec2 ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec3 uvc = vec3(uv*ofc.zw + ofc.xy, index); vec2 ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec3 uvd = vec3(uv*ofd.zw + ofd.xy, index); vec2 ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;

    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );

    return mix( mix( textureGrad( samp, uva, ddxa, ddya ),
    textureGrad( samp, uvb, ddxb, ddyb ), b.x ),
    mix( textureGrad( samp, uvc, ddxc, ddyc ),
    textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

const ivec3 OFFSET = ivec3(-1, 0, 1);
const float TextureScale = 2;
const int MAX_LAYER_COUNT = 8;

uniform int LayerCount;
uniform float LayerHeights[MAX_LAYER_COUNT - 1];
uniform float BlendValues[MAX_LAYER_COUNT - 1];
uniform sampler2DArray Textures;

uniform float MapHeight;
uniform sampler2D HeightMap;

in vec3 FragPosition;
in vec2 SamplePosition;
in float Height;

out vec4 gPosition;
out vec4 gNormal;
out vec4 gAlbedoSpec;

float getHeight(float sampledHeight) {
    return sampledHeight * MapHeight;
}

vec3 calculateNormal(vec2 samplePosition, float h) {
    //     10
    //     |
    // 01--11--21
    //     |
    //     12
	
    float s11 = getHeight(h);
	float s01 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.xy).x);
    float s10 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yx).x);

    vec3 normal = normalize(vec3(s01 - s11, 1, s10 - s11));
	
//    float s01 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.xy));
//    float s21 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.zy));
//    float s10 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yx));
//    float s12 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yz));
//
//    vec3 normal = normalize(vec3(s01 - s21, 1, s10 - s12));

    return normal;
}




vec3 triMap(int index, vec3 normal) {
    vec2 yUV = FragPosition.xz / TextureScale;
    vec2 xUV = FragPosition.zy / TextureScale;
    vec2 zUV = FragPosition.xy / TextureScale;
	
    vec3 yDiff = textureNoTile(Textures, yUV, index).xyz;
    vec3 xDiff = textureNoTile(Textures, xUV, index).xyz;
    vec3 zDiff = textureNoTile(Textures, zUV, index).xyz;

//    vec3 yDiff = texture(Textures, vec3(yUV, index)).xyz;
//    vec3 xDiff = texture(Textures, vec3(xUV, index)).xyz;
//    vec3 zDiff = texture(Textures, vec3(zUV, index)).xyz;

    vec3 blendWeights = vec3(pow(abs(normal.x), 10.0), pow(abs(normal.y), 10.0), pow(abs(normal.z), 10.0));
    // Divide our blend mask by the sum of it's components, this will make x+y+z=1
    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);
    // Finally, blend together all three samples based on the blend mask.
    return xDiff * blendWeights.x + yDiff * blendWeights.y + zDiff * blendWeights.z;
}

void main() {
    if (SamplePosition.x < 0.01 || SamplePosition.y < 0.01 ||
        SamplePosition.x > 0.99 || SamplePosition.y > 0.99) {
        discard;
    }
	
	float hTex = texture(HeightMap, SamplePosition).x;
    vec3 normal = calculateNormal(SamplePosition, hTex);

    vec3 color = vec3(0.0); //triMap(0, normal);
	
	int blendValue = 1;
	for (int i = 1; i < LayerCount - 1; i++) {
		if (hTex < LayerHeights[i])
			break;
		blendValue++;
	}
	
	float drawStrength = smoothstep(-BlendValues[blendValue] / 2, BlendValues[blendValue] / 2, hTex - LayerHeights[blendValue]);
	color = mix(triMap(blendValue,  normal), triMap(blendValue+1,  normal), drawStrength*2.0);
	
//    // smoothly interpolate between the different layers
//    for (int i = 0; i < LayerCount - 1; i++) {
//        float drawStrength = smoothstep(-BlendValues[i] / 2, BlendValues[i] / 2, Height - LayerHeights[i]);
//		if (drawStrength < 0.1)
//			continue;
//        color = mix(color, triMap(i + 1,  normal), drawStrength);
//    }

//    float slope = (1 - normal.y) * 90;
//    float threshold = 90 - 50;
//
//    float multiplier = 1 - clamp((threshold - slope) / threshold, 0, 1);
//
//    multiplier *= clamp(smoothstep(LayerHeights[1], LayerHeights[3], Height), 0, 1);
//    multiplier *= 1 - clamp(smoothstep(LayerHeights[3], 1, Height), 0, 0.8);
//    color = mix(triMap(4,  normal), color, 1 - multiplier);

    /*
    if (Height < 0.501) {
        discard;
    }
    */

    gPosition = vec4(FragPosition, 0);
    gNormal = vec4(normal, 0);
    gAlbedoSpec = vec4(color, 0.04);
    // gAlbedoSpec = vec4(0.5, 0.5, 0.5, 0);
}
