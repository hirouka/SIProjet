#version 330

// Attributs de chaque sommets de triangles (dans le maillage)
layout(location = 0) in vec3 position; 
//layout(location = 1) in vec3 normal;
//layout(location = 2) in vec3 tangent;
//layout(location = 3) in vec2 coord;

// input uniforms 
uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
//uniform mat3 normalMat;   // normal matrix
//uniform mat4 mvpDepthMat; // mvp depth matrix

// output vectors (camera space)
//out vec3 normalView;
//out vec3 tangentView;
out vec3 eyeView;
out vec2 uvcoord;
out vec4 shadowcoord; // ajout

void main() {
  float d1 = texture(textureAAfficher,position.xy).z;
  vec3 newpos = vec3(position.x, position.y, d1);
  gl_Position = projMat*mdvMat*vec4(newpos,1.0);
  //normalView  = normalize(normalMat*normal);
  //tangentView = normalize(normalMat*tangent);
  eyeView     = normalize((mdvMat*vec4(position,1.0)).xyz);
  //uvcoord     = coord*5.0;
  //shadowcoord  = mvpDepthMat *vec4(position,1)*0.5+0.5;
}
  // *** TODO: project position in light space (and send it to the fragment) to be able to compare depths ***