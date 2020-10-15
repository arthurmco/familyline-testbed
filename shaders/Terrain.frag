#version 150

uniform vec3 color;

in vec3 norm_Model;
//in vec3 norm_Camera;

in vec2 tex_coords;
flat in int tex_idx;

// The texture atlas stores image on the Y, then X direction
// How many textures are on the tex atlas, on both sides
const float ycount = 2.0;
const float xcount = 1.0;

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

//uniform LightInfo lights[4];

#include "lights.inc"

void main() {
  vec3 vcolor = diffuse_color;
  vec3 texel = vec3(1,0,0);

  vec2 uvcoords = vec2(tex_coords.x, (tex_idx/ycount) + (tex_coords.y/ycount));

  texel = texture(tex_sam, uvcoords).rgb;
  vcolor = mix(diffuse_color, texel * 0.9, tex_amount);
  vec3 vambient = mix(ambient_color, texel * 0.4, tex_amount);

  vec3 directional_color = get_directional_light_color(vcolor, dirColor, dirPower,
        -dirDirection);

  LightInfo lights[4];
  lights[0].position = vec3(20, 56, 20);
  lights[0].color = vec3(0, 0, 1);
  lights[0].strength = 1000;

  vec3 point_color = get_point_light_color(lights, lightCount, outPosition);

  vec3 finalColor = (vambient) + directional_color + point_color;

  ocolor = vec4(finalColor, 1.0);
}
