#version 330

in vec4 vertexPosition;
in vec2 UVcoords;
in vec4 normalDirection;

uniform int flag_ambient;
uniform int flag_diffuse;
uniform int flag_specular;
uniform sampler2D TextureSampler;

uniform vec4 Li[2]; // light intensity
uniform vec4 Lp[2]; // light position
uniform vec4 La[2]; // light ambient
uniform vec4 Ld[2]; // light diffuse
  
uniform vec4 Ks[2];
uniform vec4 Kd[2];
uniform vec4 Ka[2];

uniform mat4 ViewMatrix;

float specular_exponent = 1.0; // specular 'power'

smooth out vec4 outColor;

void main()
{
	vec4 interpColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 lightDirection;
	vec4 reflectDirection;
	float cosAngIncidence;	
	float phongTerm;

	vec4 viewDirection = normalize(inverse(ViewMatrix) * (vec4(0.0, 0.0, 0.0, 1.0) - vertexPosition));

	for(int i=0; i < 2; i++) {
		lightDirection = normalize(Lp[i] - vertexPosition);
		cosAngIncidence = dot(normalize(normalDirection), lightDirection);
		cosAngIncidence = clamp(cosAngIncidence, 0, 1);

		reflectDirection = reflect(-lightDirection, normalize(normalDirection));
		phongTerm = dot(viewDirection, reflectDirection);
		phongTerm = clamp(phongTerm, 0, 1);
		phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0;
		phongTerm = pow(phongTerm, specular_exponent);

		if(flag_ambient == 1)
			interpColor += La[i] * Ka[i];
		if(flag_diffuse == 1)
			interpColor += Ld[i] * Kd[i] * cosAngIncidence;
		if(flag_specular == 1)
			interpColor += Li[i] * Ks[i] * phongTerm;
	}

	outColor = interpColor * texture2D(TextureSampler, UVcoords);
}
