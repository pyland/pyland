#version 130

in vec2 f_texture_coord;
uniform sampler2D texture;

void main() {
    vec4 colour = texture2D(texture, f_texture_coord);
    
    if (colour.a == 0.0) {
        discard;
    }

    gl_FragColor.rgba   = colour.rgba;
}
