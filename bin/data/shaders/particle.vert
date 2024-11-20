#version 150

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 velocity;
in float mass;

out vec4 vColor;

void main() {
    // Color based on velocity
    float speed = length(velocity.xyz);
    vec3 color = mix(vec3(1.0, 0.5, 0.3), vec3(1.0, 1.0, 0.8), speed / 10.0);
    
    vColor = vec4(color, 1.0);
    gl_Position = modelViewProjectionMatrix * position;
    gl_PointSize = 100.0;
}
