#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){
    loadShaders();

    // if(!_mesh.load(DATA_DIR"/models/quad.off")) exit(1);
    if(!_mesh.load(DATA_DIR"/models/lemming.off")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _trackball.setCamera(&_cam);

    //Cam for solar system
    //_cam.lookAt(Vector3f(0, 4, 8), Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 1, 0));

    _cam.lookAt(Vector3f(0, 0, 3), Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 1, 0));

    glClearColor(0.5,0.5,0.5,1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _shader.activate();

  glUniform1i(_shader.getUniformLocation("wireframe"), 0);
  Vector3f worldVect = Vector3f(1, 1, 1);
  worldVect.normalize();
  Vector3f yPers = Vector3f(0, 1, 0);
  float rotationAngle = acos(yPers.dot(worldVect));
  Vector3f axis = yPers.cross(worldVect);
  axis.normalize();

  Affine3f A =  Translation3f(0, 0, 0) 
            //* AngleAxisf(rotationAngle, axis)
            * AngleAxisf(_angle, Vector3f(0, 1, 0));     
  Matrix4f M = A.matrix();


  Matrix3f linearMatrix = Affine3f(_cam.viewMatrix()).linear();
  Vector3f lightVect = (linearMatrix*Vector3f(1, 2, 2));
  Matrix3f normalMatrix = ((Affine3f(_cam.viewMatrix()*M).linear()).inverse()).transpose();

  
  glUniform3fv(_shader.getUniformLocation("light_pos"), 1, lightVect.data());
  glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normalMatrix.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_obj"), 1, GL_FALSE, M.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_proj"), 1, GL_FALSE, _cam.projectionMatrix().data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_view"), 1, GL_FALSE,  _cam.viewMatrix().data());

  _mesh.draw(_shader);


  if(_wireframe)
  {
    glUniform1i(_shader.getUniformLocation("wireframe"), 1);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDepthFunc(GL_LEQUAL);

    _mesh.draw(_shader);
    
    glUniform1i(_shader.getUniformLocation("wireframe"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDepthFunc(GL_LESS);
  }

  _shader.deactivate();
}

/*!
   callback to draw graphic primitives
 */
void Viewer::drawSceneSolarSystem()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _shader.activate();

  glUniform1i(_shader.getUniformLocation("wireframe"), 0);

  Matrix3f linearMatrix = Affine3f(_cam.viewMatrix()).linear();
  Vector3f lightVect = (linearMatrix*Vector3f(10, 10, 5));
  

  
  glUniform3fv(_shader.getUniformLocation("light_pos"), 1, lightVect.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_proj"), 1, GL_FALSE, _cam.projectionMatrix().data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_view"), 1, GL_FALSE,  _cam.viewMatrix().data());


  //DRAW SUN
  Affine3f A =  Translation3f(0, 0, 0) 
            //* AngleAxisf(rotationAngle, axis)
            * AngleAxisf(_angle, Vector3f(0, 1, 0));     
  Matrix4f M = A.matrix();
  Matrix3f normalMatrix = ((Affine3f(_cam.viewMatrix()*M).linear()).inverse()).transpose();
  


  glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normalMatrix.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_obj"), 1, GL_FALSE, M.data());
  _mesh.draw(_shader);

  const float radius = 0.5f;
  float rotationX = glfwGetTime() * radius;


  //DRAW EARTH
  Vector3f earthRotation = Vector3f(0, 1-sin(0.08726646), 1-cos(0.8726646));
  earthRotation.normalize();
  A = AngleAxisf(rotationX, earthRotation)
      *  Translation3f(3, 0, 0)
      * AngleAxisf(rotationX*4, Vector3f(0, 1, 0));

  M = A.matrix() * Scaling(Vector4f(0.5, 0.5, 0.5, 1));
  normalMatrix = ((Affine3f(_cam.viewMatrix()*M).linear()).inverse()).transpose();
  


  glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normalMatrix.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_obj"), 1, GL_FALSE, M.data());
  _mesh.draw(_shader);


  //DRAW MOON
  A = AngleAxisf(rotationX, earthRotation)
      *  Translation3f(3, 0, 0)
      * AngleAxisf(rotationX*4, earthRotation)
      * Translation3f(-1, 0, 0)
      * AngleAxisf(rotationX*8,Vector3f(0, 1, 0));

  M = A.matrix() * Scaling(Vector4f(0.15, 0.15, 0.15, 1));
  normalMatrix = ((Affine3f(_cam.viewMatrix()*M).linear()).inverse()).transpose();
  


  glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normalMatrix.data());
  glUniformMatrix4fv(_shader.getUniformLocation("M_obj"), 1, GL_FALSE, M.data());
  _mesh.draw(_shader);

  if(_wireframe)
  {
    glUniform1i(_shader.getUniformLocation("wireframe"), 1);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDepthFunc(GL_LEQUAL);

    _mesh.draw(_shader);
    
    glUniform1i(_shader.getUniformLocation("wireframe"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDepthFunc(GL_LESS);
  }

  _shader.deactivate();
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene2D()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _shader.activate();

  Matrix4f M;

  //Controle clavier
  /*M <<  _zoom, 0, 0, _offset.x(),
        0, _zoom, 0, _offset.y(),
        0, 0, _zoom, 0,
        0, 0, 0, 1;*/

  M <<  0.5, 0, 0, -0.6,
        0, 0.5, 0, -1,
        0, 0, 0.5, 0,
        0, 0, 0, 1;

  glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
  
  _mesh.draw(_shader);

  M <<  -0.5, 0, 0, 0.6,
        0, 0.5, 0, -1,
        0, 0, 0.5, 0,
        0, 0, 0, 1;

  glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
  
  _mesh.draw(_shader);

  Affine3f A = Translation3f(0, 0.5, 0)
              * AngleAxisf(_angle, Vector3f(0, 0, 1))
              * Translation3f(0, -0.5, 0);
                

  M = A.matrix();

  glUniformMatrix4fv(_shader.getUniformLocation("M"), 1, GL_FALSE, M.data());
  
  _mesh.draw(_shader);



  _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
    drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      _offset.y() += 0.05f;
    }
    else if (key==GLFW_KEY_DOWN)
    {
      _offset.y() -= 0.05f;
    }
    else if (key==GLFW_KEY_LEFT)
    {
      _offset.x() -= 0.05f;
    }
    else if (key==GLFW_KEY_RIGHT)
    {
      _offset.x() += 0.05f;
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
      _zoom *= 1.1f;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
      _zoom /= 1.1f;
    }
    else if(key == GLFW_KEY_KP_ADD)
    {
      _angle+=0.5;
    }
    else if(key == GLFW_KEY_KP_SUBTRACT)
    {
      _angle-=0.5;
    }
    else if (key == GLFW_KEY_W)
    {
      _wireframe = !_wireframe;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1f*y);
}

void Viewer::charPressed(int /*key*/)
{
}
