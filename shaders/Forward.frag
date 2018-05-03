#version 150

uniform vec3 color;

in vec3 norm_Model;
in vec3 norm_Camera;
in vec2 tex_coords;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform float tex_amount;

out vec4 ocolor;

uniform sampler2D tex_sam;

void main() {
  vec3 vcolor = diffuse_color;

  vec3 texel = vec3(1,0,0);
  texel = texture(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel, tex_amount);
  
  vec3 lightDirection = vec3(80, 200, 80);
  float lightPower = 1;

  //Cosine of angle between normal and light direction
  vec3 n = normalize(norm_Model);

  vec3 l = normalize(lightDirection);
  float cosTheta = clamp(dot(n, l), 0, 1);
  
  vec3 finalColor = (ambient_color) + (vcolor * cosTheta * lightPower);

  ocolor = vec4(finalColor, 1.0);
}
