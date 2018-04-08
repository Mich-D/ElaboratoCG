#version 420

in vec3 position;
in vec3 normal;

out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 projMtx;
uniform mat4 viewMtx;
uniform mat4 modelMtx;
uniform vec3 lightPos_worldspace;

void main(void)
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projMtx * viewMtx * modelMtx * vec4(position, 1.0);

	// Position of the vertex, in worldspace : M * position
	position_worldspace = (modelMtx * vec4(position, 1.0)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 position_camera = ( viewMtx * modelMtx * vec4(position,1)).xyz;
	eyeDirection_cameraspace = vec3(0,0,0) - position_camera;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 lightPos_cameraspace = ( viewMtx * vec4(lightPos_worldspace, 1.0)).xyz;
	lightPos_cameraspace = lightPost_cameraspace + eyeDirection_cameraspace;

	// Normal of the the vertex, in camera space
	normal_cameraspace = ( viewMtx * modelMtx * vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
}