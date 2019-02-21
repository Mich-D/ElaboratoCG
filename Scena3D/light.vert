#version 420
in vec3 aPos;

uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 projMtx;

void main()
{
    gl_Position = projMtx * viewMtx * modelMtx * vec4(aPos, 1.0);
}