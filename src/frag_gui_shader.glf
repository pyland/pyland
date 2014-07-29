#version 130
// precision mediump float;
in vec2 v_texCoord;
uniform sampler2D s_texture;
void main() 
{
  //TODO: Really hacky - just for the demo
  if(v_texCoord.r == 0.0)
                  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  else
  discard;
//  vec4 colour =   texture2D(s_texture, v_texCoord);
//  if(colour.a == 0.0) discard;
//  gl_FragColor = colour;
}
