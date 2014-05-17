#version 330

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

out vec3 position_eye, normal_eye;

void main()
{
  position_eye = vec3 (ViewMatrix*ModelMatrix*vec4(Position, 1.0));
  //normal_eye = vec3 (ViewMatrix*ModelMatrix*vec4(Normal, 0.0));
  normal_eye = vec3 (ModelMatrix*vec4(Normal, 0.0));
  gl_Position = ProjectionMatrix * vec4(position_eye, 1.0);
}
