#version 400

in vec3 position;

uniform mat4 projMat;

void main(void)
{
	gl_Position = projMat * vec4(position, 1.0);
}