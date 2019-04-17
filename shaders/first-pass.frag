#version 330

in  vec3  normalView;
in  vec3  eyeView;
in  vec2  uvcoord;
in  float depth;

// the 2 output locations: refers to glDrawBuffers in the cpp file 
layout(location = 0) out vec4 outBuffer1;
out vec4 outBuffer;

uniform vec3 color;

void main() {
  vec3 n = normalize(normalView);
  outBuffer = vec4(n,depth); 
  //outBuffer = vec4(1.,0.,0.,0.); 
  
}
