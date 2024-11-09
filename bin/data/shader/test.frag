OF_GLSL_SHADER_HEADER

uniform vec4 globalColor;
uniform float time;

in vec3 vnorm;
in vec2 varyingtexcoord;

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
    vec2 uv = varyingtexcoord * 2.0 - 1.0;  // Adjust UV range to [-1, 1] for more natural scaling

    // Plasma granulation texture using animated noise
    float granulation = plasmaGranulation(uv, 10.0);

    // Sunspot areas: darker regions using lower-frequency noise
    float sunspot = sunspotMask(uv, 0.7);

    // Solar flares: bright regions that burst randomly
    float flare = solarFlare(uv);

    // Combine base granulation, sunspots, and flares into the final color
    vec3 plasmaColor = mix(vec3(1.0, 0.8, 0.5), vec3(1.0, 1.0, 0.8), granulation);
    plasmaColor = mix(plasmaColor, vec3(0.2, 0.1, 0.1), sunspot);      // Darker sunspots
    plasmaColor += vec3(1.0, 0.5, 0.2) * flare;                        // Add bright flare color

    // Emissive shading for a glowing sun effect
    plasmaColor *= 1.5;  // Brightness boost for glow

    outputColor = vec4(plasmaColor, 1.0);
}
