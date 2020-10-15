// Get the color resulted by the main directional light refleting
// into the object
vec3 get_light_color(vec3 diffusecolor, vec3 lightColor, float lightPower,
    vec3 lightDirection) {

    //Cosine of angle between normal and light direction
    vec3 n = normalize(norm_Model);

    vec3 l = normalize(lightDirection);
    float cosTheta = clamp(dot(n, l), 0, 1);

    return (diffusecolor + cosTheta * lightPower * lightColor);
}

// Get the color resulted by the main directional light refleting
// into the object
vec3 get_directional_light_color(vec3 diffusecolor, vec3 lightColor, float lightPower,
    vec3 lightDirection) {

    //Cosine of angle between normal and light direction
    vec3 n = normalize(norm_Model);

    vec3 l = normalize(lightDirection);
    float cosTheta = clamp(dot(n, l), 0, 1);

    return (diffusecolor * cosTheta * lightPower * lightColor);
}

vec3 get_point_light_color() {
    vec3 finalColor = vec3(0, 0, 0);

    for (int i = 0; i < lightCount; i++) {
	float attenConstant = 0;
	float attenLinear = 0;
	float attenExp = 1;
	float dist = length(outlights[i].ldirection);

	float lightPower =  (attenConstant + attenLinear * dist +
			     attenExp * dist * dist);

	vec3 lightColor = get_light_color(finalColor, outlights[i].color,
					  outlights[i].strength, outlights[i].ldirection);

	finalColor += (lightColor / lightPower);
    }

    return finalColor;
}
