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

/// Color, power and direction for the directional lights
uniform vec3 dirColor;
uniform float dirPower;
uniform vec3 dirDirection;

int lightCount = 1;
in vec4 outPosition;

#include "lights.inc"

void main() {
  vec3 vcolor = diffuse_color;
  vec3 texel = vec3(1,0,0);

  texel = texture(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel * 0.9, tex_amount);
  vec3 vambient = mix(ambient_color, texel * 0.4, tex_amount);

  LightInfo lights[4];
  lights[0].position = vec3(20, 56, 20);
  lights[0].color = vec3(0, 0, 1);
  lights[0].strength = 1000;

  vec3 directional_color = get_directional_light_color(vcolor, dirColor, dirPower,
        -dirDirection);
  vec3 point_color = get_point_light_color(lights, lightCount, outPosition);

  vec3 finalColor = (vambient) + directional_color + point_color;

  ocolor = vec4(finalColor, 1.0);
}
