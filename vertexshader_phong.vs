#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

//uniform int lightCount;
//uniform vec3 lightPosition[5];
//uniform vec4 lightIntensity[5];
//uniform vec4 ambientIntensity[5];

//uniform vec4 diffuseColor;
//uniform float shininessFactor;

//uniform bool ambient;
//uniform bool diffuse;
//uniform bool specular;
//uniform bool visualizeLight;

//uniform vec4 lightModelColor;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out vec3 normalPosition;
out vec3 viewPosition;

// fixed point light properties
vec3 light_position_world  = vec3 (10.0, 10.0, 10.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour
  
// surface reflectance
vec3 Ks = vec3 (1.0, 1.0, 1.0); // fully reflect specular light
vec3 Kd = vec3 (1.0, 0.5, 0.0); // orange diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0); // fully reflect ambient light
float specular_exponent = 100.0; // specular 'power'

void main()
{
	vec4 vertexPosition = ModelMatrix * vec4(Position, 1.0);
	gl_Position = ProjectionMatrix*ViewMatrix*vertexPosition;
	vec3 normalPosition = vec3(ModelMatrix * vec4(Normal, 0.0));
	vec3 viewPosition = vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0) - vertexPosition);
}
