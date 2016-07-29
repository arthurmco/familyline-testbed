#version 120

varying vec3 norm_Camera;
varying vec3 light_Camera;
varying vec2 tex_coords;

varying vec3 normal_Camera;
varying vec3 eyeDir_Camera;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform float tex_amount;

uniform mat4 mModel, mView;

/* We support only 4 light influences per scene for now */
uniform int lightCount;
uniform float lightStrenghts[4];
uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];

uniform sampler2D tex_sam;

void main() {

  vec3 dcolor = diffuse_color;

  vec3 texel = vec3(0,0,0);
  texel += texture2D(tex_sam, tex_coords).rgb;
  dcolor = mix(diffuse_color, texel, tex_amount);

  vec3 acolor = dcolor * 0.1;

  vec4 finalColor = vec4(acolor,1);
  vec4 lightSum = vec4(0,0,0,0);

  for (int i = 0; i < lightCount; i++) {

    vec3 light_World = lightPositions[i];
    vec3 lightPos_Camera = (mView * vec4(light_World, 1.0)).xyz;
    vec3 lightDir_Camera = lightPos_Camera + eyeDir_Camera;

    vec3 n = normalize(normal_Camera);
    vec3 l = normalize(lightDir_Camera);
    float dist = 10.0f;

    //Cosine of angle between normal and light direction
    float cosTheta = clamp(dot(n, l), 0, 1);
    vec4 lightColor = vec4(0);
    lightColor.rgb = (lightColors[i] * lightStrenghts[i] / (dist*dist));
    lightColor.a = cosTheta;
    lightSum += (lightColor * cosTheta);
  }

  lightSum = clamp(lightSum,0,1);

  finalColor.rgb += mix(dcolor*0.9, lightSum.rgb*2, lightSum.a);
  finalColor.a = 1;

  //finalColor = vec4(lightSum.a,lightSum.a,lightSum.a,1);
  gl_FragColor = finalColor;
}
