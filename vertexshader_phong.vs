#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform sampler2D TextureSampler;

out vec4 vertexPosition;
out vec4 vertexColor;
out vec4 viewDirection;
out vec4 normalDirection;

void main()
{
	vertexPosition = ModelMatrix * vec4(Position, 1.0);
	normalDirection = ModelMatrix * vec4(Normal, 1.0);

	gl_Position = ProjectionMatrix*ViewMatrix*vertexPosition;
	vertexColor = texture2D(TextureSampler, UV);
}
