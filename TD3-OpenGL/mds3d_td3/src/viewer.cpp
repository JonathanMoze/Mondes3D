#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _zoom(1), _mode(0), _activeMode(0), _coord(Vector2f(0, 0))
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

    if(!_mesh.load(DATA_DIR"/models/monkey.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _trackball.setCamera(&_cam);
    glClearColor(0.6, 0.6, 0.6, 1);
    glEnable(GL_DEPTH_TEST);
    
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
  glUniform1f(_shader.getUniformLocation("zoom"), _zoom);
  glUniform1i(_shader.getUniformLocation("mode"), _mode);
  glUniform2fv(_shader.getUniformLocation("coord"), 1, _coord.data());
  _mesh.draw(_shader);
  if(_activeMode){
    _mode=1;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glUniform1i(_shader.getUniformLocation("mode"), _mode);
    _mesh.draw(_shader);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_LINE_SMOOTH);
    _mode=0;
  }
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
  if(key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    if (_activeMode==0)
    {
      _activeMode=1;
    } else {
      _activeMode=0;
    }
    
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      _coord.y() += 0.1;
    }
    else if (key==GLFW_KEY_DOWN)
    {
      _coord.y() -= 0.1;
    }
    else if (key==GLFW_KEY_LEFT)
    {
      _coord.x() -= 0.1;
    }
    else if (key==GLFW_KEY_RIGHT)
    {
      _coord.x() += 0.1;
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
      _zoom *=2;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
      _zoom /=2;
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
  _cam.zoom(-0.1*y);
}

void Viewer::charPressed(int /*key*/)
{
}
