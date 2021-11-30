#version 330 core

const ivec3 OFFSET = ivec3(-1, 0, 1);
const uint MAX_VALUE = 65535u;
const float TextureScale = 10;

const int LAYER_COUNT = 6;
const vec3 COLORS[LAYER_COUNT] = vec3[](vec3(0, 0, 204) / 255, vec3(252, 208, 70) / 255, vec3(51, 205, 0) / 255,
                                        vec3(32, 129, 0) / 255, vec3(68, 68, 68) / 255, vec3(255, 250, 250) / 255);
const float LAYER_HEIGHTS[LAYER_COUNT - 1] = float[](0.5, 0.68, 0.66, 0.73, 0.9);
const float BLEND_VALUES[LAYER_COUNT - 1] = float[](0.0, 0.1, 0.1, 0.1, 0.2);

uniform usampler2D HeightMap;
uniform sampler2D GroundTexture;
uniform float MapHeight;

in vec3 FragPosition;
in vec2 SamplePosition;
in float Height;

out vec4 gPosition;
out vec4 gNormal;
out vec4 gAlbedoSpec;

vec3 calculateLayerColor(float height) {
    vec3 color = COLORS[0];

    for (int i = 0; i < LAYER_COUNT - 1; i++) {
        // smoothly interpolate between the different layers
        float drawStrength = smoothstep(-BLEND_VALUES[i] / 2, BLEND_VALUES[i] / 2, height - LAYER_HEIGHTS[i]);

        color = mix(color, COLORS[i + 1], drawStrength);
    }

    return color;
}

float getHeight(uvec4 sampledHeight) {
    return sampledHeight.x / float(MAX_VALUE) * MapHeight;
}

vec3 calculateNormal(vec2 samplePosition) {
    //     10
    //     |
    // 01--11--21
    //     |
    //     12
    float s01 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.xy));
    float s21 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.zy));
    float s10 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yx));
    float s12 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yz));

    vec3 normal = normalize(vec3(s21 - s01, 2, s12 - s10));

    return normal;
}


vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)),
                                                2.0+dot(p,vec2(11.0,47.0)),
                                                3.0+dot(p,vec2(41.0,29.0)),
                                                4.0+dot(p,vec2(23.0,31.0))))*103.0);
}

// https://www.iquilezles.org/www/articles/texturerepetition/texturerepetition.htm
vec4 textureNoTile( sampler2D samp, in vec2 uv )
{
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
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;

    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );

    return mix( mix( textureGrad( samp, uva, ddxa, ddya ),
    textureGrad( samp, uvb, ddxb, ddyb ), b.x ),
    mix( textureGrad( samp, uvc, ddxc, ddyc ),
    textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

void main(){
    vec3 normal = calculateNormal(SamplePosition);

    vec3 color = calculateLayerColor(Height);

    vec2 yUV = FragPosition.xz / TextureScale;
    vec2 xUV = FragPosition.zy / TextureScale;
    vec2 zUV = FragPosition.xy / TextureScale;

    vec3 yDiff = textureNoTile(GroundTexture, yUV).xyz;
    vec3 xDiff = textureNoTile(GroundTexture, xUV).xyz;
    vec3 zDiff = textureNoTile(GroundTexture, zUV).xyz;

    vec3 blendWeights = vec3(pow(abs(normal.x), 10.0), pow(abs(normal.y), 10.0), pow(abs(normal.z), 10.0));
    // Divide our blend mask by the sum of it's components, this will make x+y+z=1
    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);
    // Finally, blend together all three samples based on the blend mask.
    color = xDiff * blendWeights.x + yDiff * blendWeights.y + zDiff * blendWeights.z;

    gPosition = vec4(FragPosition, 0);
    gNormal = vec4(normal, 0);
    gAlbedoSpec = vec4(color, 0);
}
