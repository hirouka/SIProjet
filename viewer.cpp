#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
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
  //deleteFBO();

}
/*void Viewer::initFBO() {
 //A COMPLETER
}*/

/*void Viewer::createFBO() {
  // Ids needed for the FBO and associated textures 
  glGenFramebuffers(1,&_fbo);
  glGenTextures(1,&_rendNormalId);

}*/
//-----------------------------------------------
//Creation de notre géométrie de notre TERRAIN (la grille)
void Viewer::createVAO() {

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoTerrain);
  glGenVertexArrays(1,&_vaoQuad);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);

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
  //glDrawElements(GL_TRIANGLES,2,GL_UNSIGNED_INT,(void *)0); //On va dessiner 2 triangles ici
  glDrawArrays(GL_TRIANGLES, 0,6);
  glBindVertexArray(0); //On dissocie le VAO à la géométrie
}
//-----------------------------------------------
//INUTILE POUR L'INSTANT
void Viewer::drawVAOPerlin() { 
  glBindVertexArray(_vaoQuad); //On va associer un VAO à notre future géométrie
  glDrawElements(GL_TRIANGLES,2,GL_UNSIGNED_INT,(void *)0); //On va dessiner 2 triangles ici
  glBindVertexArray(0); //On dissocie le VAO à la géométrie
}

//-----------------------------------------------
//On va créer nos shaders, ici faut ajouter noise.vert/frag
void Viewer::createShaders() {
  _vertexFilenames.push_back("shaders/noise.vert");
  _fragmentFilenames.push_back("shaders/noise.frag");
}
//-----------------------------------------------
//Fonction à priori useless ? (permet de passer des variables à nos shaders)
void Viewer::enableShader(unsigned int shader) {
  // current shader ID 
  GLuint id = _shaders[shader]->id(); 

  // activate the current shader 
  glUseProgram(id);

  // send the model-view matrix 
  //glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));

  // send the projection matrix 
  //glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));

  // send the normal matrix (top-left 3x3 transpose(inverse(MDV))) 
  //glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  // send a light direction (defined in camera space)
  // TODO (use the variable _light)
    //glUniform3f(glGetUniformLocation(id,"light"),_light[0],_light[1],_light[2]);


}

void Viewer::disableShader() {
  // desactivate all shaders 
  glUseProgram(0);
}

//-----------------------------------------------
//Création de la scène
void Viewer::paintGL() {
  glViewport(0,0,512,512); //ETAPE 1.1 : Créer la fenêtre (viewport) qui a affichera la texture
  glClear(GL_COLOR_BUFFER_BIT); // Effacer ce qu'il y avait sur l'écran auparavant
  enableShader(_currentshader); //Active le shader n°0 (celui de Perlin)
  drawVAOCarre(); //dessin de la géométrie (carré) qui servira de support à la texture 
  disableShader(); //Va désactiver TOUS les shaders 
 
  //Eventuellement ajouter ici le FBO
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

  // space: next shader
  if(ke->key()==Qt::Key_Space) {
    _currentshader = (_currentshader+1)%_shaders.size();
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

  // VAO creation 
  createVAO();

  // starts the timer 
  _timer->start();
}

