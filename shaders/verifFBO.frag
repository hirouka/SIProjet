#version 330

out vec4 outBuffer;

uniform sampler2D texperlin;

void main() {
  outBuffer = texelFetch(texperlin,ivec2(gl_FragCoord.xy),0);
  //outBuffer = vec4(1.0,0,0,0);
}
