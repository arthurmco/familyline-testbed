#version 120

uniform vec3 color;

varying vec3 norm_Camera;
varying vec3 light_Camera;
varying vec2 tex_coords;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform float tex_amount;

uniform sampler2D tex_sam;

void main() {
  vec3 n = normalize(norm_Camera);
  vec3 l = normalize(light_Camera);

  //Cosine of angle between normal and light direction
  float cosTheta = clamp(dot(n, l), 0, 1);
  float dist = 5.0f;
  float lightPower = 30;

  vec3 vcolor = diffuse_color;

  vec3 texel = vec3(1,0,0);
  texel = texture2D(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel, tex_amount); 

  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 finalColor = ambient_color +
        (vcolor * lightColor * lightPower * cosTheta / (dist*dist));

  gl_FragColor = vec4(finalColor, 1.0);
}
