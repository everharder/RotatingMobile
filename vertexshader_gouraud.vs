#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

//uniform int num_lights;
uniform vec4 Lp[2]; // light position
uniform vec4 Ls[2]; // light specular
uniform vec4 Ld[2]; // light diffuse
uniform vec4 La[2]; // light ambient
  
uniform vec4 Ks[2];
uniform vec4 Kd[2];
uniform vec4 Ka[2];

float specular_exponent = 100.0; // specular 'power'

smooth out vec4 interpColor;

void main()
{
	vec4 vertexPosition = ModelMatrix * vec4(Position, 1.0);
	gl_Position = ProjectionMatrix*ViewMatrix*vertexPosition;

	vec4 lightDirection;
	vec4 reflectDirection;
	float cosAngIncidence;
	float phongTerm;
	interpColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec4 normalDirection = normalize(ModelMatrix * vec4(Normal, 0.0));
	vec4 viewDirection = normalize(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0) - vertexPosition);

	for(int i=0; i < 2; i++) {
		interpColor = interpColor + La[i] * Ka[i];

		lightDirection = normalize(Lp[i] - vertexPosition);
		cosAngIncidence = dot(normalDirection, lightDirection);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);

		interpColor = interpColor + Ld[i] * Kd[i] * cosAngIncidence;

		reflectDirection = reflect(-lightDirection, normalDirection);
		phongTerm = dot(viewDirection, reflectDirection);
		phongTerm = clamp(phongTerm, 0, 1);

		/**< No specular highlight if the light is on the wrong side */
		//phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0;

		phongTerm = pow(phongTerm, specular_exponent);

		interpColor = interpColor + Ls[i] * Ks[i] * phongTerm;
	}
}
