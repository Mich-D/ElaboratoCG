#version 420

in vec3 position;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;

void main(void)
{
	gl_Position = projMtx * viewMtx * modelMtx * vec4(position, 1.0);
}