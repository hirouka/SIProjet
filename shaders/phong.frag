#version 330

in  vec4 fragmentColor;
in vec3  vnormal; // AJOUT : ajout de vnormal

out vec4 bufferColor;

uniform mat3 normalMat; // AJOUT
uniform vec3 light; // AJOUT : diriger la lumière avec la souris

void main() {
  

  //vec3 l = normalize(vec3(1.0,0.0,0.0)); // AJOUT : Vecteur de lumière
  vec3 l = normalize(light);
  vec3 n = normalize(vnormal);
  vec3 r = reflect(l,n);
  vec3 e = vec3(0.0,0.0,-1.0);

  float ec = 30.0; // AJOUT : intensité lumineuse

  vec3 pa = vec3(0.8,0.1,0.2); // AJOUT : couleur d'ambiance (rouge)
  vec3 pd = vec3(0.2,0.1,0.0); // AJOUT : couleur de la lumière (vert)
  vec3 ps = vec3(1.0,1.0,1.0); // AJOUT : couleur tâche lumineuse (bleu)

  float diff = max(dot(n,l),0.0);
  vec4 matColor = vec4(vec3(diff*pd), 1.0);
  
  float gloss = max(dot(r,e), 0.0);
  vec4 glossColor = vec4(vec3(ps*pow(gloss,ec)),1.0);

  bufferColor = vec4(pa, 1.0) + matColor + glossColor;
}
