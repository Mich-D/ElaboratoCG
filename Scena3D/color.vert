#version 420

in vec3 position;

uniform mat4 MVP;

void main(void)
{
	gl_Position = MVP * vec4(position, 1.0);
}