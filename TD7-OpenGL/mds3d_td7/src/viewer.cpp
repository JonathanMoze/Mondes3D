#include "viewer.h"
#include "camera.h"
#include "SOIL2.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _theta(0), _rotate(false)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){

    // Background color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    loadShaders();
    //if(!_mesh.load(DATA_DIR"/models/earth2.obj")) exit(1);
    if(!_mesh.load(DATA_DIR"/models/bs_rest.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _cam.setPerspective(M_PI/3,0.1f,20000.0f);
    _cam.lookAt(Vector3f(0,1,4), Vector3f(0,0,0), Vector3f(0,1,0));
    _trackball.setCamera(&_cam);

/*
    diffuse_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/diffuse.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
	  );
    if( 0 == diffuse_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    normals_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/normals.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
	  );
    if( 0 == normals_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
*/

/*
    earth_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/earth.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == earth_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    earthnormal_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/earth_normal.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == earthnormal_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
*/


   ogre_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/diffuse_ogre.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == ogre_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    ogrenormal_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/ogre_normals.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == ogrenormal_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

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
    // configure the rendering target size (viewport)
    glViewport(0, 0, _winWidth, _winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader.activate();
/*  
    //Active diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_int);
    glUniform1i(_shader.getUniformLocation("diffuse_texture"), 0);


    //Active normals map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normals_int);
    glUniform1i(_shader.getUniformLocation("normals_map"), 1);
*/
/*
    //Active earth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earth_int);
    glUniform1i(_shader.getUniformLocation("diffuse_texture"), 0);


    //Active earth normals map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthnormal_int);
    glUniform1i(_shader.getUniformLocation("normals_map"), 1);

*/
    //Active ogre texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ogre_int);
    glUniform1i(_shader.getUniformLocation("diffuse_texture"), 0);


    //Active ogre normals map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ogrenormal_int);
    glUniform1i(_shader.getUniformLocation("normals_map"), 1);


    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"),1,GL_FALSE,_cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"),1,GL_FALSE,_cam.projectionMatrix().data());

    Affine3f M(AngleAxisf(_theta,Vector3f(0,1,0)));

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"),1,GL_FALSE,M.matrix().data());

    Matrix4f matLocal2Cam = _cam.viewMatrix() * M.matrix();
    Matrix3f matN = matLocal2Cam.topLeftCorner<3,3>().inverse().transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"),1,GL_FALSE,matN.data());

    Vector3f lightDir = Vector3f(1,0,1).normalized();
    lightDir = (_cam.viewMatrix().topLeftCorner<3,3>() * lightDir).normalized();
    glUniform3fv(_shader.getUniformLocation("lightDir"),1,lightDir.data());

    _mesh.draw(_shader);

    _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
  if(_rotate)
    _theta += 0.01*M_PI;
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
  else if(key == GLFW_KEY_A && action == GLFW_PRESS)
  {
    _rotate = !_rotate;
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      if(_magFilter == GL_NEAREST){
        _magFilter = GL_LINEAR;
        printf("mag : LINEAR\n");
      } else {
        _magFilter = GL_NEAREST;
        printf("mag : NEAREST\n");
      }
    }
    else if (key==GLFW_KEY_DOWN)
    {
      if(_minFilter == GL_LINEAR_MIPMAP_LINEAR){
        _minFilter = GL_NEAREST_MIPMAP_NEAREST;
        printf("min : NEAREST_MIPMAP_NEAREST\n");
      }
      else if(_minFilter != GL_LINEAR_MIPMAP_LINEAR){
        _minFilter = GL_LINEAR_MIPMAP_LINEAR;
        printf("min : LINEAR_MIPMAP_LINEAR\n");
      } 
    }
    else if (key==GLFW_KEY_LEFT)
    {
      if(_minFilter != GL_LINEAR){
        _minFilter = GL_LINEAR;
        printf("min : LINEAR\n");
      }
    }
    else if (key==GLFW_KEY_RIGHT)
    {
      if(_minFilter != GL_NEAREST){
        _minFilter = GL_NEAREST;
        printf("min : NEAREST\n");
      }
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
      if(_minFilter != GL_LINEAR_MIPMAP_NEAREST){
        _minFilter = GL_LINEAR_MIPMAP_NEAREST;
        printf("min : LINEAR_MIPMAP_NEAREST\n");
      }
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
      if(_minFilter != GL_NEAREST_MIPMAP_LINEAR){
        _minFilter = GL_NEAREST_MIPMAP_LINEAR;
        printf("min : NEAREST_MIPMAP_LINEAR\n");
      }
    }
  }
}

void Viewer::charPressed(int /*key*/)
{
}
