#version 120

uniform vec3 color;

varying vec3 norm_Camera;
varying vec3 light_Camera;


uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;


void main() {
  vec3 n = normalize(norm_Camera);
  vec3 l = normalize(light_Camera);

  //Cosine of angle between normal and light direction
  float cosTheta = clamp(dot(n, l), 0, 1);
  float dist = 5.0f;
  float lightPower = 50;

  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 finalColor = ambient_color +
        (diffuse_color * lightColor * lightPower * cosTheta / (dist*dist));

  gl_FragColor = vec4(finalColor, 1.0);
}
