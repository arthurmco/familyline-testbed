//  -*- mode: glsl-mode;-*-

struct LightInfo {
    mediump vec3 position;
    mediump vec3 color;
    lowp float strength;
};

// Get the color resulted by the light refleting into the object
mediump vec3 get_light_color(mediump vec3 diffusecolor, mediump vec3 lightColor, mediump float lightPower,
                     mediump vec3 lightDirection) {

    //Cosine of angle between normal and light direction
    mediump vec3 n = normalize(norm_Model);

    mediump vec3 l = normalize(lightDirection);
    mediump float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    return (diffusecolor + cosTheta * lightPower * lightColor);
}

// Get the color resulted by the main directional light refleting
// into the object
mediump vec3 get_directional_light_color(mediump vec3 diffusecolor, mediump vec3 lightColor, mediump float lightPower,
                                 mediump vec3 lightDirection) {

    //Cosine of angle between normal and light direction
    mediump vec3 n = normalize(norm_Model);

    mediump vec3 l = normalize(lightDirection);
    mediump float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    return (diffusecolor * cosTheta * lightPower * lightColor);
}

mediump vec3 get_point_light_color(LightInfo[4] lights, int lightCount, mediump vec4 vertexPos) {
    mediump vec3 finalColor = vec3(0, 0, 0);

    for (int i = 0; i < lightCount; i++) {
        mediump vec3 ldirection = lights[i].position - vertexPos.xyz;
        
        lowp float attenConstant = 1.0;
	lowp float attenLinear = 0.5;
        lowp float attenExp = 0.25;
        mediump float dist = length(ldirection);

        mediump float lightPower =  (attenConstant + attenLinear * dist +
                             attenExp * dist * dist);

        mediump vec3 lightColor = get_light_color(finalColor, lights[i].color,
                                          lights[i].strength, ldirection);

	finalColor += (lightColor / lightPower);
    }

    return finalColor;
}
