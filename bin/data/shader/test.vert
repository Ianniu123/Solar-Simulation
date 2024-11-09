OF_GLSL_SHADER_HEADER
// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;
// this is the end of the default functionality

// output to frag shader; make sure names and types match up
out vec3 vnorm;
out vec2 varyingtexcoord;

// the time value is passed into the shader by the OF app.
uniform float time;

void main()
{
    vec4 modifiedPosition = modelViewProjectionMatrix * position;
	gl_Position = modifiedPosition;
	
    varyingtexcoord = texcoord;
}