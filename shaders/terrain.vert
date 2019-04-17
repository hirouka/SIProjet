#version 330

// Attributs de chaque sommets de triangles (dans le maillage)
layout(location = 0) in vec3 position; 

// input uniforms 
uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
//uniform mat4 mvpDepthMat; // mvp depth matrix

// output vectors (camera space)
out vec3 normalView;
out vec3 eyeView;
out vec2 uvcoord;
out vec3 tangentView;
//out vec4 shadowcoord; // ajout

out vec3 position_world;
out vec4 pos;
out float u;
out float v;

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

  //eliminer le terrain en dessous de niveau la mer 
  if(texture(textureAAfficher,vec2(u,v)).y>0.35) {
      	p.z = texture(textureAAfficher,vec2(u,v)).z;
        
  }else{
      	p.z=0.35;
  }
  //normalView = normalize(normalMat*);
  //tangentView = normalize(normalMat*vec3(1.0,1.0,0.0));
  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
  normalView  = normalize(normalMat*getNormal());
  gl_Position = projMat*mdvMat*vec4(p,1.0);
  pos = gl_Position;

}
