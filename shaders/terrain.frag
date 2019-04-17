#version 330

in  vec3  normalView;
//in  vec3  tangentView;
in  vec3  eyeView;
in  vec2  uvcoord;
//in vec4 shadowcoord; // ajout

in float u;
in float v;
in vec3 tangentView;


uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform vec3 light; // ajut de lumière 

out vec4 outBuffer;
in vec4 pos; 


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



void main() {

    //PARAMETREDE PHONG
    const vec3 ambient  = vec3(0.7,0.3,0.0);
    const vec3 diffuse  = vec3(0.3,0.4,0.0);
    const vec3 specular = vec3(0.8,0.2,0.2);
    float et = 10.0;

  // calcul de la tbn 
    vec3 n = normalize(normalView);
    vec3 t = normalize(tangentView);
    vec3 b = normalize(cross(n,t));
    mat3 tbn = mat3(t,b,n);

    //PHONG MODEL
    vec3 l = normalize(light);
////////////////////////////////////////////////////////////////////////
  //  float diff = max(dot(l,n),0.);
  //  float spec = pow(max(dot(reflect(l,n),e),0),et);
  // outBuffer = vec4(n,0.) *(diff + spec)*2.0;
////////////////////////////////////////////////////////////////////////


    outBuffer =choisirCouleur(texture(textureAAfficher,vec2(u,v)));;
    vec2 coordsout = vec2(u,v); 
////////////////////////////////////////////////////////////////////////
    //vec3 color = ambient + diff*diffuse + spec*specular;
    //bufferColor = texture(simple,coordsout*2);
////////////////////////////////////////////////////////////////////////

  /*  vec4 textureN = texture(texNormal,coordsout);
    vec3 normTex = textureN.xyz*2.0-1.0;
    vec3 newNorm = tbn*normTex;

*/

////////////////////////////////////////////////////////////////////////
    //vec3 n = normalize(normalView);//
    //vec3 e = normalize(eyeView);//
    //vec3 l = normalize(light);//

////////////////////////////////////////////////////////////////////////

    //n = normalize(newNorm);
   /* vec3 e = normalize(eyeView);
    //l*=vec3(-1.0,1.0,1.0);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    outBuffer = *(diff*spec);

*/





   // outBuffer =  (ambient,0.0) + diff*textureN*textureN + (spec*specular,0.0);

    


    //outBuffer = texelFetch(textureAAfficher,ivec2(gl_FragCoord.xy),0);
    //outBuffer = vec4(0.0,1,0,0);
   // outBuffer = vec4(n,0.);
}


