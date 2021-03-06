#version 330

smooth in vec4 outColor;
in vec2 UVcoords;

uniform sampler2D myTextureSampler;
uniform float alpha;

out vec4 outputColor;

void main()
{
	outputColor = outColor * texture2D(myTextureSampler, UVcoords);
	outputColor.a = alpha;
}
