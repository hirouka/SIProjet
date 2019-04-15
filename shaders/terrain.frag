#version 330

in  vec3  normalView;
//in  vec3  tangentView;
in  vec3  eyeView;
//in  vec2  uvcoord;
in vec4 shadowcoord; // ajout

uniform sampler2D textureAAfficher; //Texture de perlin qui défini la hauteur à chaque point
uniform sampler2D texNormal; //Texture de normal

uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform vec3 light; // ajut de lumière 

out vec4 outBuffer;

void main() {

    /*float et     = 10.0;
    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = max(dot(l,n),0.);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);
    outBuffer = vec4(0.0,1,0,0)*(diff + spec)*2.0;*/


    //outBuffer = texelFetch(textureAAfficher,ivec2(gl_FragCoord.xy),0);
    //outBuffer = vec4(0.0,1,0,0);
    outBuffer = vec4(normalView*0.5+0.5,0.);
}


