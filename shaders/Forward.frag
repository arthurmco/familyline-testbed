#version 150

uniform vec3 color;

in vec3 norm_Model;
//in vec3 norm_Camera;

in vec2 tex_coords;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform float tex_amount;

out vec4 ocolor;

uniform sampler2D tex_sam;

uniform int lightCount;
struct LightOut {
    vec3 ldirection;
    vec3 color;
    float strength;
};
in LightOut outlights[4];


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

void main() {
  vec3 vcolor = diffuse_color;
  vec3 texel = vec3(1,0,0);

  texel = texture(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel * 0.9, tex_amount);
  vec3 vambient = mix(ambient_color, texel * 0.4, tex_amount);

  vec3 directional_color = get_directional_light_color(vcolor, vec3(1, 1, 1), 1,
        vec3(60, 200, 240));
  vec3 point_color = get_point_light_color();

  vec3 finalColor = (vambient) + directional_color + point_color;

  ocolor = vec4(finalColor, 1.0);
}
