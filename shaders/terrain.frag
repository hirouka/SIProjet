#version 330

in  vec3  normalView;
in  vec3  eyeView;
in  vec2  uvcoord;
//in vec4 shadowcoord; // ajout
//in  vec3  tangentView;

in float u;
in float v;

uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform vec3 light; // ajut de lumière 

out vec4 outBuffer;
in vec4 pos; 

//----------------------------------------
vec4 choisirCouleur(vec4 t) {
  vec4 a;
  if(t.x<0.3) {
  	a=t+vec4(0.1,0.3,0.4,1.0);//********eau***//
  }else{
    if(t.x<0.5){
  	  a=t*0.7+vec4(0.0,0.2,0.1,1.0);//*****forêt*****//
  	}else{
  	  if(t.x<0.9){
  	    a=t*0.5+vec4(0.4,0.35,0.3,1.0);//******montagne*********//
  	  }else{
  	    a=t*0.3+vec4(0.6,0.6,1.0,1.0);//*******neige*********//
  	  }
  	}
  }
  return a;
}
//----------------------------------------
void main() {

    //PARAMETREDE PHONG
    float et = 20.0;
    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    //PHONG MODEL
    ////////////////////////////////////////////////////////////////////////
    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0),et);
    vec4 colorMountain = choisirCouleur(texture(textureAAfficher,vec2(u,v))); //couleur des montagnes (neige,eau...)
    outBuffer =colorMountain*(diff + spec)*2.0;
    ////////////////////////////////////////////////////////////////////////

}


