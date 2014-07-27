#version 130

in vec2 f_texture_coord;
uniform sampler2D texture;
uniform vec4 colour;

void main() {
    float alpha = texture2D(texture, f_texture_coord).r;
    
    // if (alpha == 0.0) {
    //     discard;
    // }

    // gl_FragColor.rg  = f_texture_coord.xy;
    // gl_FragColor.b = 1.0;
    // gl_FragColor.rgb = vec3(1.0,1.0,1.0);
    gl_FragColor.rgb   = colour.rgb;
    gl_FragColor.a   = colour.a * alpha;
    // gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}
