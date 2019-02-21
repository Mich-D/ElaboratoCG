#version 420

in vec3 aPos;
in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 projMtx;

void main()
{
    FragPos = vec3(modelMtx * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMtx))) * aNormal;  
    
    gl_Position = projMtx * viewMtx * vec4(FragPos, 1.0);
}