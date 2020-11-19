#version 150

uniform sampler2D texRender;
uniform sampler2D texGUI;

in mediump vec2 uv;
out mediump vec4 color;

void main() {

    mediump vec4 crender = texture(texRender, uv);
    mediump vec4 cgui = texture(texGUI, vec2(uv.x, 1.0-uv.y));

    color = vec4(mix(crender, cgui, cgui.a).rgb, 1.0);
}
