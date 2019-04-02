#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");

  // load a mesh into the CPU memory
  //_mesh = new Mesh(filename);
  _grid = new Grid();

  // create a camera (automatically modify model/view matrices according to user interactions)
 // _cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));
  _cam  = new Camera();
  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
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
  glGenFramebuffers(1,&_fbo); //Déclaration du FBO
  glGenTextures(1,&_texPerlin); //Déclaration d'une texture

  //Paramétrage de la texture
  glBindTexture(GL_TEXTURE_2D,_texPerlin); //Activation de la texture
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,512,512,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER,_fbo); //Activation du FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texPerlin,0); //Attachement de la texture au FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"


  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;
}
//-----------------------------------------------
void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo);
  glDeleteTextures(1,&_texPerlin);
}
//-----------------------------------------------
//Creation de notre géométrie de notre CARRE
void Viewer::createVAOCarre() {

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

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

  glGenBuffers(2,_terrain);
  glGenVertexArrays(1,&_vaoTerrain);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
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
//On va créer nos shaders, ici faut ajouter noise.vert/frag
void Viewer::createShaders() {
  _vertexFilenames.push_back("shaders/noise.vert");
  _fragmentFilenames.push_back("shaders/noise.frag");

  _vertexFilenames.push_back("shaders/verifFBO.vert");
  _fragmentFilenames.push_back("shaders/verifFBO.frag");
}

//-----------------------------------------------
//permet de passer des variables à nos shaders
void Viewer::enableShaderPerlin() {

  GLuint id = _shaders[0]->id(); 
  glUseProgram(id);

}
//-----------------------------------------------
void Viewer::enableShaderVerifFBO(){

  GLuint id = _shaders[1]->id(); 

  //Envoi de la texture au shader VerifFBO
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

  //ETAPE 1.2 : Créer la FBO
  
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo); //On active le mode "écriture en texture"

  //ETAPE 1.1 : Créer la fenêtre (viewport) qui a affichera la texture
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderPerlin();
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders 
 
 glBindFramebuffer(GL_FRAMEBUFFER,0); //On désactive le mode "écriture en texture"

//ETAPE1.3 : Vérification qu'on a bien notre FBO
  glViewport(0,0,512,512); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShaderVerifFBO(); //On va utiliser le shader qui affiche le FBO (donc créer new shader)
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders 

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
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
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
  createFBOPerlin();
//-----------------------------------------------

  // starts the timer 
  _timer->start();
}

