#version 150

uniform mat4 modelViewProjectionMatrix;
uniform float T_core;
uniform float alpha;
uniform float target_radius;

in vec4 position;
in vec4 velocity;
in float mass;

out vec4 vColor;

void main() {
    // Color based on velocity
    float dist = length(position);
    float normalized_radius = (target_radius - dist) / target_radius;
    float temperature = T_core * pow(normalized_radius, alpha);
    
    vec3 coreColor = vec3(1.0, 1.0, 1.0);
    vec3 hotColor = vec3(1.0, 1.0, 0.8);
    vec3 coldColor = vec3(1.0, 0.5, 0.3);

    vec3 color;

    if (temperature > T_core * 0.6) {
        color = coreColor;
    } else {
        color = mix(coldColor, hotColor, temperature / T_core);
    }
    
    vColor = vec4(color, 1.0);
    gl_Position = modelViewProjectionMatrix * position;
    gl_PointSize = 1.0;
}
