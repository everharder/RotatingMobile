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

out vec3 normalDirection;
out vec3 viewPosition;

void main()
{
	vec4 vertexPosition = ModelMatrix * vec4(Position, 1.0);
	gl_Position = ProjectionMatrix*ViewMatrix*vertexPosition;
	vec3 normalDirection = normalize(vec3(ModelMatrix * vec4(Normal, 0.0)));
	vec3 viewPosition = vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0) - vertexPosition);
}
