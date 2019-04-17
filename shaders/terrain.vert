#version 330

// Attributs de chaque sommets de triangles (dans le maillage)
layout(location = 0) in vec3 position; 
//layout(location = 1) in vec3 texNormal;
//layout(location = 2) in vec3 tangent;
//layout(location = 3) in vec2 coord;

// input uniforms 
uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
//uniform mat4 mvpDepthMat; // mvp depth matrix

// output vectors (camera space)
out vec3 normalView;
//out vec3 tangentView;
out vec3 eyeView;
out vec2 uvcoord;
//out vec4 shadowcoord; // ajout
out vec4 pos;
out float u;
out float v;
out vec3 tangentView ;
//----------------------------------------------
//Permet de récupérer la normale dans la texture associée
vec3 getNormal(){
    return texture(texNormal,position.xy*0.5+0.5).xyz;
}
//----------------------------------------------
void main() {

  u=position.x*0.5+0.5;
  v=position.y*0.5+0.5;
  vec3 p = position;
  if(texture(textureAAfficher,vec2(u,v)).x>0.35) {
      	p.z = texture(textureAAfficher,vec2(u,v)).x;
  }else{
      	p.z=0.35;
  }


  //normalView = normalize(normalMat*);
  tangentView = normalize(normalMat*vec3(1.0,1.0,0.0));




  
  //float d1 = texture(textureAAfficher,position.xy*0.5+0.5).z;
 // vec3 newpos = vec3(p.x, p.y, d1);
//  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
  normalView  = normalize(normalMat*getNormal());

  gl_Position = projMat*mdvMat*vec4(p,1.0);
  //tangentView = normalize(normalMat*tangent);
  pos = gl_Position;
  //uvcoord     = coord*5.0;
  //shadowcoord  = mvpDepthMat *vec4(position,1)*0.5+0.5;
}
