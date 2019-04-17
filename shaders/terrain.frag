#version 330

in  vec3  normalView;
in  vec3  eyeView;
in  vec2  uvcoord;
//in vec4 shadowcoord; // ajout
//in  vec3  tangentView;

uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform vec3 light; // ajut de lumière 
uniform float anim;

out vec4 outBuffer;
in vec4 pos; 

//----------------------------------------
vec4 choisirCouleur(vec4 texture) {
  vec4 color;
  if(texture.x<0.4) {
    //*************************eau***************************//
  	color=texture+vec4(0.1,0.4,0.4,1.0);
  }else{
   
    if(texture.x<0.5){
       //*********************forest*************************//
  	  color=texture*0.7+vec4(0.0,0.3,0.1,1.0);
  	}else{
  	  if(texture.x<0.9){
        //*********************montagne************************//
  	    color=texture*0.5+vec4(0.4,0.4,0.3,1.0);
  	  }else{
        //*************************neige**********************//
  	    color=texture*0.3+vec4(0.6,0.7,1.0,1.0);
  	  }
  	}
  }
  return color;
}
//----------------------------------------

vec4 shade(in vec2 coord) {
  vec4  nd = texture(texNormal,coord,0);
  vec4 colorMountain = choisirCouleur(texture(textureAAfficher,uvcoord)); //couleur des montagnes (neige,eau...)
  vec4 c =colorMountain;
  
  vec3 n = normalize(normalView);
  float d = nd.z; //Profondeur

  vec3 e = vec3(0,0,-0.2);
  //vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  float diff = max(dot(l,n),0.0);
  float spec = pow(max(dot(reflect(l,n),e),0.0),d*10.0);
  
  //vec4 S = vec4(c.xyz*(diff+spec),1);   //AJOUTE : S est pour "shade" et prend en compte l'ombrage
  vec4 S = colorMountain*(diff + spec)*2.0;
  vec4 F = vec4(vec3(0.2,0.1,0.1),1); // AJOUTE : F pour "Fog", et donne la couleur du brouillard

  return mix(S,F,clamp(0,1.,d)); //AJOUTE : On mix les deux filtres, "clamp" permet de mettre d (la profondeur) entre 0 et 1
}



//----------------------------------------

void main() {
    vec4 color = shade(uvcoord);
    outBuffer = vec4(color);

    ///////////////////////VERSION SANS BROUILLARD//////////////////////////
    /*//PARAMETREDE PHONG
    float et = 10.0;
    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    //PHONG MODEL
    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0),et);
    vec4 colorMountain = choisirCouleur(texture(textureAAfficher,uvcoord)); //couleur des montagnes (neige,eau...)
    outBuffer =colorMountain*(diff + spec)*1.5;*/
    ////////////////////////////////////////////////////////////////////////

}


