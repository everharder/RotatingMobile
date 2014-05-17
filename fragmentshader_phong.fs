#version 330

smooth in vec3 normalPosition;
smooth in vec3 viewPosition;

out vec4 outputColor;

void main()
{
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

    	outputColor = interpColor;
}
