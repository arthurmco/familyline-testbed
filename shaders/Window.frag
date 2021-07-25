#version 150

uniform sampler2D texRender;
uniform sampler2D texGUI;

in vec2 uv;
out vec4 color;

void main() {

    vec4 crender = texture(texRender, uv);
    vec4 cgui = texture(texGUI, vec2(uv.x, 1.0-uv.y));

    color = vec4(mix(crender, cgui, cgui.a).rgb, 1.0);
}
