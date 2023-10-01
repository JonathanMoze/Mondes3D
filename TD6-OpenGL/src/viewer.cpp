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
    if(!_mesh.load(DATA_DIR"/models/cow.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _cam.setPerspective(M_PI/3,0.1f,20000.0f);
    _cam.lookAt(Vector3f(0,1,4), Vector3f(0,0,0), Vector3f(0,1,0));
    _trackball.setCamera(&_cam);

    /*earth_int = SOIL_load_OGL_texture
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

    clouds_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/earth_clouds.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == clouds_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    night_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/earth_night.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == night_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }*/

    checkerBoard_int = SOIL_load_OGL_texture
	  (
		DATA_DIR"/textures/checkerboard.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	  );
    if( 0 == night_int)
    {
      printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }


    int texW = 256;
    int texH = 256;
    unsigned char colors[6][3] = { {255,  0 , 0}, {0, 255,  0 }, { 0,  0, 255},
                                  {255, 255, 0}, {0, 255, 255}, {255, 0, 255} };
    unsigned char data[256*256*3];

    glGenTextures(1,&color_int);
    glBindTexture(GL_TEXTURE_2D, color_int);
    for(int l=0; ; ++l) {
        for(int i=0; i<texW*texH; ++i){
            data[3*i  ] = colors[l][0];
            data[3*i+1] = colors[l][1];
            data[3*i+2] = colors[l][2];
        }
        glTexImage2D(GL_TEXTURE_2D, l, (GLint)GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if(texW==1 && texH==1)
          break;
        texW = std::max(1, texW/2);
        texH = std::max(1, texH/2);
    }


    glGenSamplers(1, &_samplerId);
    

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

    glSamplerParameteri(_samplerId, GL_TEXTURE_MIN_FILTER, _minFilter);
    glSamplerParameteri(_samplerId, GL_TEXTURE_MAG_FILTER, _magFilter);

/*
    //Active earth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earth_int);
    glBindSampler(0, _samplerId);
    glUniform1i(_shader.getUniformLocation("earthTexture"), 0);


    //Active clouds texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, clouds_int);
    glBindSampler(1, _samplerId);
    glUniform1i(_shader.getUniformLocation("cloudsTexture"), 1);

    //Active earth night texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, night_int);
    glBindSampler(2, _samplerId);
    glUniform1i(_shader.getUniformLocation("earthNightTexture"), 2);


    //Active checkerboard texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, checkerBoard_int);
    glBindSampler(0, _samplerId);
    glUniform1i(_shader.getUniformLocation("checkerBoardTexture"), 0);
    */
    
    //Active color texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_int);
    glBindSampler(0, _samplerId);
    glUniform1i(_shader.getUniformLocation("mipmap_color_texture"), 0);


    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"),1,GL_FALSE,_cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"),1,GL_FALSE,_cam.projectionMatrix().data());

    Affine3f M(AngleAxisf(_theta,Vector3f(0,1,0)));

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"),1,GL_FALSE,M.matrix().data());

    Matrix4f matLocal2Cam = _cam.viewMatrix() * M.matrix();
    Matrix3f matN = matLocal2Cam.topLeftCorner<3,3>().inverse().transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"),1,GL_FALSE,matN.data());

    Vector3f lightDir = Vector3f(1,0,1).normalized();
    lightDir = (matLocal2Cam.topLeftCorner<3,3>() * lightDir).normalized();
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
