#version 330

// input attributes 
layout(location = 0) in vec3 position;// position of the vertex in world space

uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)
uniform mat3 normalMat; // normal matrix (constant for all the vertices)

uniform sampler2D texNormal; //Texture de normal
// output variables that will be interpolated during rasterization (equivalent to varying)
out vec3 normalView;
out vec3 eyeView;
out vec2 uvcoord;

out float depth; // IMPORTANT

//----------------------------------------------
//Permet de récupérer la normale dans la texture associée
vec3 getNormal(){
    return texture(texNormal,position.xy*0.5+0.5).xyz;
}
//----------------------------------------------
void main() {

  uvcoord     = vec2(position.x,position.y)*4.0;
  //gl_Position = projMat*mdvMat*vec4(position,1.0);
  gl_Position = vec4(position,1);
  normalView  = normalize(normalMat*getNormal());
  eyeView     = normalize((mdvMat*vec4(position,1.0)).xyz);
  depth       = -(mdvMat*vec4(position,1.0)).z/30.0;

}
