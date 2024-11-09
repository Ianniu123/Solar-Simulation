#version 150
OF_GLSL_SHADER_HEADER

uniform vec4 globalColor;
uniform float time;

in vec3 vnorm;

out vec4 outputColor;

// Simple Perlin-like noise function for GLSL 1.50 compatibility
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// Smooth Perlin noise function using GLSL 1.50-compatible code
float perlinNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Plasma Granulation Simulation
float plasmaGranulation(vec2 uv, float scale) {
    return perlinNoise(uv * scale + time * 0.2);
}

// Sunspot Simulation using low-frequency noise for darker areas
float sunspotMask(vec2 uv, float intensity) {
    float spot = perlinNoise(uv * 2.0 + time * 0.05); // Lower frequency for larger shapes
    return smoothstep(0.4, 0.6, spot) * intensity;
}

// Solar Flare Simulation for bright, dynamic flares
float solarFlare(vec2 uv) {
    float flareNoise = perlinNoise(uv * 10.0 - time * 0.3);
    return step(0.9, flareNoise);  // Threshold for random flares
}

void main()
{
vec4 c1 = vec4(0.05,0.2,0.3,1.0);
vec4 c2 = vec4(0.4,0.6,0.7,1.0);
vec4 c3 = vec4(0.8,0.9,1.0,1.0);

	vec3 L = vec3(0.707,0.0,0.707);
	float t = dot(L,vnorm);

	vec4 color = mix(mix(c1, c2, step(0.5, t)), c3, step(0.9, t));
//        vec4 color = mix(mix(c1, c2, smoothstep(0.25, 0.5, t)), c3, smoothstep(0.75, 0.9, t));
    outputColor = color;

}
