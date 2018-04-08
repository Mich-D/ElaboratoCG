#version 420

in vec3 position_worldspace;
in vec3 normal_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;

out vec3 colorOut;

uniform mat4 modelViewMtx;
uniform vec3 lightPos_worldspace;

void main(void)
{
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;

	// Material properties
	vec3 MaterialDiffuseColor = vec3(0.8,0.1,0.1);
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	//vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length( lightPos_worldspace - position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( lightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(eyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);

	//colorOut = vec4(0.8, 0.2, 0.2, 1.0);
}