#version 150

in vec3 position;
in vec2 in_uv;
out vec2 uv;

void main() {
    uv = in_uv;
 
    vec3 fixpos = vec3((position.x * 2.0) - 1,
		       (position.y * 2.0) - 1, position.z);
    
    gl_Position = vec4(fixpos, 1.0);
}
