#version 330

//in  vec3  normalView;
//in  vec3  tangentView;
//in  vec3  eyeView;
//in  vec2  uvcoord;
in vec4 shadowcoord; // ajout

uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix

out vec4 outBuffer;

void main() {
 // outBuffer = texelFetch(textureAAfficher,ivec2(gl_FragCoord.xy),0);
outBuffer = vec4(0.0,1,0,0);
}


