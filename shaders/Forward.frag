#version 150

uniform mediump vec3 color;

in mediump vec3 norm_Model;
//in mediump vec3 norm_Camera;

in mediump vec2 tex_coords;

uniform mediump vec3 diffuse_color;
uniform lowp float diffuse_intensity;
uniform mediump vec3 ambient_color;
uniform lowp float ambient_intensity;
uniform lowp float tex_amount;

out mediump vec4 ocolor;

uniform sampler2D tex_sam;

/// Color, power and direction for the directional lights
uniform mediump vec3 dirColor;
uniform lowp float dirPower;
uniform mediump vec3 dirDirection;

#include "lights.inc"

uniform LightInfo lights[4];
uniform int lightCount;
in mediump vec4 outPosition;

void main() {
  mediump vec3 vcolor = diffuse_color;
  mediump vec3 texel = vec3(1,0,0);

  texel = texture(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel * 0.95, tex_amount);
  mediump vec3 vambient = mix(ambient_color, texel * 0.001, tex_amount);

  mediump vec3 directional_color = get_directional_light_color(vcolor, dirColor, dirPower,
        -dirDirection);
  mediump vec3 point_color = get_point_light_color(lights, lightCount, outPosition);

  mediump vec3 finalColor = (vambient) + directional_color + point_color;

  mediump vec3 gamma = vec3(1.0/2.2);  
  ocolor = vec4(pow(finalColor, gamma), 1.0);
}
