#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>


#include "camera.h"
#include "meshLoader.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(
	 const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
 void createFBOPerlin(); //AJOUTE E1.2
  void createVAOCarre(); //AJOUTE E1.1
  void createVAOTerrain(); //AJOUTE E1.1
  void deleteVAO();
  void deleteFBO(); //AJOUTE E1.2
  void drawVAOCarre();
  void drawVAO();

  void createShaders();
  void enableShaderPerlin();
  void enableShaderVerifFBO();
  void disableShader();

  QTimer        *_timer;    // timer that controls the animation
  

 // Mesh   *_mesh;   // the mesh
  Grid   *_grid;
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  std::vector<std::string> _vertexFilenames;   // all vertex filenames
  std::vector<std::string> _fragmentFilenames; // all fragment filenames
  std::vector<Shader *>    _shaders;           // all the shaders

  GLuint _vao;
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;

  GLuint _texPerlin;
  GLuint _fbo; //AJOUTE E1.2


};

#endif // VIEWER_H
