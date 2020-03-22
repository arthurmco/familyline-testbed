#version 150

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform mat4 mWorld, mView, mProjection;

out vec3 norm_out;
out vec2 tex_coords;

out vec3 norm_Model;
//out vec3 norm_Camera;

uniform int lightCount;
struct LightInfo {
    vec3 position;
    vec3 color;
    float strength;
};
uniform LightInfo lights[4]; // max lights is 4

struct LightOut {
    vec3 ldirection;
    vec3 color;
    float strength;
};
out LightOut outlights[4];
    
void main() {
  mat4 mvp = mProjection * mView * mWorld;
  gl_Position = mvp * vec4(position, 1.0);

  vec3 pos_World = (mWorld * vec4(position, 1.0)).xyz;
  vec3 pos_Camera = (mView * mWorld * vec4(position, 1.0)).xyz;
  vec3 eyeDir_Camera = vec3(0,0,0) - pos_Camera;

  for (int i = 0; i < lightCount; i++) {
      vec3 distance_World = lights[i].position - pos_World;

      outlights[i].ldirection = distance_World;
      outlights[i].color = lights[i].color;
      outlights[i].strength = lights[i].strength;
  }

  norm_Model = (mWorld * vec4(normal, 0.0)).xyz;
  vec3 normal_Camera = (mView * mWorld * vec4(normal, 0.0)).xyz;

  //norm_Camera = normal_Camera;
  tex_coords = texcoord;
}
