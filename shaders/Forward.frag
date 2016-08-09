#version 150

uniform vec3 color;

in vec3 norm_Camera;
in vec2 tex_coords;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform float tex_amount;

/* We support only 4 light influences per scene for now */
uniform int lightCount;
uniform float lightStrenghts[4];
uniform vec3 lightColors[4];

in vec3 lightDirections[4];

uniform sampler2D tex_sam;

void main() {
  vec3 vcolor = diffuse_color;

  vec3 texel = vec3(1,0,0);
  texel = texture(tex_sam, tex_coords).rgb;
  vcolor = mix(diffuse_color, texel, tex_amount);

  vec4 lightSum = vec4(0);

  //Cosine of angle between normal and light direction
  vec3 n = normalize(norm_Camera);
  for (int i = 0; i < lightCount; i++) {
       vec3 l = normalize(lightDirections[i]);
       float cosTheta = clamp(dot(n, l), 0, 1);
       float dist = length(lightDirections[i]);
       float lightPower = lightStrenghts[i];
       vec3 lightColor = lightColors[i];
       lightSum.xyz = max(lightSum.xyz, (lightColor * lightPower * cosTheta / (dist*dist)));
       lightSum.a = max(lightSum.a, cosTheta);
  }

  vec3 finalColor = ambient_color + (vcolor * lightSum.a) + (lightSum.xyz * lightSum.a);

  gl_FragColor = vec4(finalColor, 1.0);
}
