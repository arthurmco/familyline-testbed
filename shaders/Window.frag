#version 150

uniform sampler2D texRender;
//uniform sampler2D texGUI;

in vec2 uv;

void main() {

    vec4 crender = texture(texRender, uv);
    //vec4 cgui = texture(texGUI, uv);

    //gl_FragColor = vec4(mix(crender, cgui, cgui.a).rgb, 1.0);
    gl_FragColor = crender;
}
