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

smooth out vec4 interpColor;

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

	/*
	//If a lightposition is visualized, set the color to the light intensity and skip the rest of the shader
	if(visualizeLight) {
		interpColor = lightModelColor;
		return;
	}*/

	vec3 lightDirection;
	vec3 reflectDirection;
	float cosAngIncidence;
	float phongTerm;
	interpColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec3 normalDirection = normalize(normalize(vec3(ModelMatrix * vec4(Normal, 0.0))));
	vec3 viewDirection = normalize(vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0) - vertexPosition));

	interpColor = interpColor + vec4(La * Ka, 1.0);

	lightDirection = normalize(light_position_world - vec3(vertexPosition));
	cosAngIncidence = dot(normalDirection, lightDirection);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	interpColor = interpColor + vec4(Ld * Kd * cosAngIncidence, 1.0);

	reflectDirection = reflect(-lightDirection, normalDirection);
	phongTerm = dot(viewDirection, reflectDirection);
	phongTerm = clamp(phongTerm, 0, 1);

	/**< No specular highlight if the light is on the wrong side */
	phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0;

	phongTerm = pow(phongTerm, specular_exponent);

	interpColor = interpColor + vec4(Ls * Ks * phongTerm, 1.0);
}
