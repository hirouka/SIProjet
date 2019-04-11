#version 330

out vec4 outBuffer;

uniform sampler2D textureAAfficher;

void main() {
  //outBuffer = texelFetch(textureAAfficher,ivec2(gl_FragCoord.xy),0);
   outBuffer = vec4(0.0,1,0,0);
}
