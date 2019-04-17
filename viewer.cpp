#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(char * filename,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _light(glm::vec3(0,0,1)),
    _mode(false),
    _var(0.0f),
    _speed(0.01f) {
    setlocale(LC_ALL,"C");

  // load a mesh into the CPU memory
  _mesh = new Mesh(filename);
  _grid = new Grid();

  // create a camera (automatically modify model/view matrices according to user interactions)
  _cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));
  //_cam  = new Camera();
  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _mesh;
  delete _cam;
  delete _grid;
  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }
  
  deleteVAO();
  deleteFBO();

}

//-----------------------------------------------
void Viewer::createFBOPerlin() {

  //ETAPE 1.2 : Créer la FBO
  glGenFramebuffers(1,&_fbo[0]); //Déclaration du FBO
  glGenTextures(1,&_texPerlin); //Déclaration d'une texture

  //Paramétrage de la texture
  glBindTexture(GL_TEXTURE_2D,_texPerlin); //Activation de la texture
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,512,512,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]); //Activation du FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texPerlin,0); //Attachement de la texture au FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"


  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;
    

//************************ajout texture normal***************************//
  
  glGenFramebuffers(1,&_fbo[1]); //Déclaration du FBO (On fait ça dans un AUTRE FBO.)
  glGenTextures(1,&_texNormal); //Déclaration d'une texture

  glBindTexture(GL_TEXTURE_2D,_texNormal); //Activation de la texture
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,512,512,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[1]); //Activation du FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texNormal,0); //Attachement de la texture au FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"

  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;
    

  //************************récupération de depth***************************//
  glGenFramebuffers(1,&_fbo[2]); //Déclaration du FBO (On fait ça dans un AUTRE FBO.)
  glGenTextures(1,&_rendDepthId); //Déclaration d'une texture
  
  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width(),height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[2]); //Activation du FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_rendDepthId,0); //Attachement de la texture au FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0); 
  
  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;
    
  /*NOTA BENE : GL_COLOR_ATTACHMENTX sert dans le cas où on a plusieurs textures pour UN SEUL FBO. Dans ce cas, on spécifie quel GL_COLOR_ATTACHMENTX (=quelle texture) on veut utiliser*/

}
//-----------------------------------------------
void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo[0]);
  glDeleteTextures(1,&_texPerlin);
  //*********ajout supr text normal ******
  glDeleteFramebuffers(1,&_fbo[1]);
  glDeleteTextures(1,&_texNormal);
  glDeleteTextures(1,&_rendDepthId);
}
//-----------------------------------------------
//Creation de notre géométrie de notre CARRE
void Viewer::createVAOCarre() {

  const GLfloat quadData[] = {-1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoQuad);
  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);

}
//-----------------------------------------------
//Creation de notre géométrie de notre TERRAIN (la grille)
void Viewer::createVAOTerrain() {

  glGenVertexArrays(1,&_vaoTerrain); //Créer VAO
  glGenBuffers(2,_terrain); //Créer 2 VBO associés (potition + normal)

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain); //activation du VAO

  //POUR POSITION
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);
}
//-----------------------------------------------
void Viewer::deleteVAO() {
  // delete your VAO here (function called in destructor)
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}
//-----------------------------------------------
//Permet de dessiner la géométrie (un carré)
void Viewer::drawVAOCarre() { 
  glBindVertexArray(_vaoQuad); //On va associer un VAO à notre future géométrie
  //glDrawElements(GL_TRIANGLES,2,GL_UNSIGNED_INT,(void *)0); //Ca pour dessiner triangles avec attributs (non adapté)
  glDrawArrays(GL_TRIANGLES, 0,6); //Ca pour dessiner des soupes de triangles
  glBindVertexArray(0); //On dissocie le VAO à la géométrie
}
//-----------------------------------------------
//Permet de dessiner le terrain
void Viewer::drawVAOTerrain() { 

  glBindVertexArray(_vaoTerrain);

  //glDrawArrays(GL_TRIANGLES, 0,_grid->nbFaces()); //Ca pour dessiner des soupes de triangles
  glDrawElements( GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0); //not sure
  glBindVertexArray(0);
}
//-----------------------------------------------
//On va créer nos shaders, ici faut ajouter noise.vert/frag
void Viewer::createShaders() {
  //_shaders[0] = Perlin
  //_shaders[1] = vérification de FBO
  //_shaders[2] = Normales
  //_shaders[3] = Terrain
  //_shaders[4] = Depth (pour le fog)
  _vertexFilenames.push_back("shaders/noise.vert");
  _fragmentFilenames.push_back("shaders/noise.frag");

  _vertexFilenames.push_back("shaders/verifFBO.vert");
  _fragmentFilenames.push_back("shaders/verifFBO.frag");

  _vertexFilenames.push_back("shaders/normal.vert");
  _fragmentFilenames.push_back("shaders/normal.frag");

  _vertexFilenames.push_back("shaders/terrain.vert");
  _fragmentFilenames.push_back("shaders/terrain.frag");

  _vertexFilenames.push_back("shaders/first-pass.vert");
  _fragmentFilenames.push_back("shaders/first-pass.frag");
  


}
//-----------------------------------------------
//permet de passer des variables à nos shaders
void Viewer::enableShaderProfondeur() {
  // get the current modelview and projection matrices 
  glm::mat4 p  = _cam->projMatrix();
  glm::mat4 mv  = _cam->mdvMatrix();
  
  GLuint id = _shaders[4]->id(); 
  glUseProgram(id);

  //Envoi de Normal à la PassProfondeur
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  glUniform1i(glGetUniformLocation(id,"texNormal"),0);

  //Envoi des matrices
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(mv[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(p[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  
}
//-----------------------------------------------
//permet de passer des variables à nos shaders
void Viewer::enableShaderPerlin() {
  GLuint id = _shaders[0]->id(); 
  glUseProgram(id);

}

//-----------------------------------------------
void Viewer::enableShaderVerifFBO(GLuint _texATester){

  GLuint id = _shaders[2]->id(); 
  //Envoi de la texture au shader VerifFBO
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texATester);
  glUniform1i(glGetUniformLocation(id,"textureAAfficher"),0);

  glUseProgram(id);

}



//---------------------Ajout *****--------------------------
void Viewer::enableShaderTerrain() {

  // get the current modelview and projection matrices 
  glm::mat4 p  = _cam->projMatrix();
  glm::mat4 mv  = _cam->mdvMatrix();

  //Envoi de la texture "normale"
  //GLuint id = _shaders[2]->id(); 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  //glUseProgram(id);
  //glUniform1i(glGetUniformLocation(id,"texNormal"),0);

  //Envoi texture Perlin
  GLuint id = _shaders[3]->id(); 
  glUseProgram(id);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texPerlin);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  
  glUniform1i(glGetUniformLocation(id,"textureAAfficher"),0);
  glUniform1i(glGetUniformLocation(id,"texNormal"),1);

  //Envoi des matrices
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(mv[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(p[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  //Ajout lumière 
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));
    glUniform1f(glGetUniformLocation(id,"anim"),_var); //AJOUT : pour modif selon le temps

}

//-----------------------------------------------
void Viewer::sendTexturePerlintoShaderNormal(){
  GLuint id = _shaders[2]->id(); 
  //Envoi de Perlin au shader "Normal" pour y récupérer les normales
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texPerlin);
  glUniform1i(glGetUniformLocation(id,"texperlin"),0);
  glUseProgram(id);
}
//-----------------------------------------------
void Viewer::disableShader() {
  // desactivate all shaders 
  glUseProgram(0);
}

//-----------------------------------------------
//Création de la scène
void Viewer::paintGL() {
  //ETAPE 1.3 : Créer FBO pour normal
  //GLenum bufferlist [] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};//liste des buffers 
  glDrawBuffer(GL_COLOR_ATTACHMENT0); //Sélection du buffer 0 (pour perlin) et l'afficher
  
  //ETAPE 1.2 : Créer la FBO pour Perlin
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]); //On active le mode "écriture en texture"

  //ETAPE 1.1 : Créer la fenêtre (viewport) qui a affichera la texture (Perlin)
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderPerlin();
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders 
  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"

  //ETAPE1.3 : Vérification qu'on a bien notre FBO (Perlin)
  /*glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderVerifFBO(_texPerlin); //On va utiliser le shader qui affiche le FBO (donc créer new shader)
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders */

  //************************création texture normale***************************//

  //ETAPE 1.4 : Envoi de shader Perlin pour créer normal
  glDrawBuffer(GL_COLOR_ATTACHMENT0); //Sélection du buffer 0 (pour perlin) et l'afficher
  
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[1]); //On active le mode "écriture en texture"
  
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sendTexturePerlintoShaderNormal();
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders

  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"

  //ETAPE1.5 : Vérification qu'on a bien notre FBO (Normal)
  /*glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderVerifFBO(_texNormal); //On va utiliser le shader qui affiche le FBO (donc créer new shader)
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders*/ 

  //************************récupération de depth***************************//
  //ETAPE 5 : Récupération de profondeur
  glDrawBuffer(GL_COLOR_ATTACHMENT0); //Sélection du buffer 0 (pour perlin) et l'afficher
  
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[2]); //On active le mode "écriture en texture"
  
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  enableShaderProfondeur();
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders

  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"

  //ETAPE1.5 : Vérification qu'on a bien notre FBO (Normal)
  /*glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderVerifFBO(_rendDepthId); //On va utiliser le shader qui affiche le FBO (donc créer new shader)
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders*/

  //************************création du maillage***************************//
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  enableShaderTerrain();
  //enableShaderVerifFBO(_texNormal);
  drawVAOTerrain();//dessin du maillage
  disableShader(); //Va désactiver TOUS les shaders



  _var += _speed;

  

}














//-----------------------------------------------
//-------PAS TOUCHER (ON S'EN FOUT)--------------
//-----------------------------------------------

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }

    
  }


  updateGL();
}

void Viewer::initializeGL() {

  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }


  // init OpenGL settings
  glClearColor(0.,0.1,0.1,1.0);
  glEnable(GL_DEPTH_TEST);

  //------------------------
  /*const GLfloat _fogdensity = 0.3f; //AJOUTE : fog
  const GLfloat _fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
  glEnable(GL_FOG); //AJOUTE : fog
  glFogi(GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2
  glFogfv(GL_FOG_COLOR, _fogColor); //set the fog color to our color chosen above
  glFogf(GL_FOG_DENSITY, _fogdensity); //set the density to the value above
  glHint(GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
  *///------------------------

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();  

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }
  
//-----------------------------------------------
//A MODIFIER

  // VAO creation 
  createVAOCarre();
  createVAOTerrain();
//-----------------------------------------------
  createFBOPerlin();

  // starts the timer 
  _timer->start();
}

