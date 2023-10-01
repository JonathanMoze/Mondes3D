
#include "camera.h"
#include "opengl.h"

using namespace Eigen;

Camera::Camera()
{
  mViewMatrix.setIdentity();
  setPerspective(float(M_PI/2.),0.1f,10000.f);
  setViewport(0,0);
}

void Camera::lookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
{
  mTarget = target;
  Vector3f d= (position-target);
  d.normalize();
  Vector3f r = up.cross(d);
  r.normalize();
  Vector3f camUp = d.cross(r);
  camUp.normalize();

  mViewMatrix << 
    r.x(),      r.y(),      r.z(),      -r.dot(position),
    camUp.x(),  camUp.y(),  camUp.z(),  -camUp.dot(position),
    d.x(),      d.y(),      d.z(),      -d.dot(position),
    0,          0,          0,          1;

}

void Camera::setPerspective(float fovY, float near, float far)
{
  m_fovY = fovY;
  m_near = near;
  m_far = far;
}

void Camera::setViewport(int width, int height)
{
  mVpWidth = width;
  mVpHeight = height;
}

void Camera::zoom(float x)
{
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  mViewMatrix = Affine3f(Translation3f(Vector3f(0,0,x*t.norm())).inverse()) * mViewMatrix;
}

void Camera::rotateAroundTarget(float angle, Vector3f axis)
{
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  Vector3f tInv = Vector3f(-t.x(), -t.y(), -t.z());
  Affine3f A = Translation3f(t) * AngleAxisf(angle, axis) * Translation3f(tInv);
  Matrix4f M = A.matrix();
  
  mViewMatrix = M * mViewMatrix;
}

Camera::~Camera()
{
}

const Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

Matrix4f Camera::projectionMatrix() const
{
  float aspect = float(mVpWidth)/float(mVpHeight);
  float theta = m_fovY*0.5f;
  float range = m_far - m_near;
  float invtan = 1.f/std::tan(theta);

  Matrix4f projMat;
  projMat.setZero();
  projMat(0,0) = invtan / aspect;
  projMat(1,1) = invtan;
  projMat(2,2) = -(m_near + m_far) / range;
  projMat(2,3) = -2 * m_near * m_far / range;
  projMat(3,2) = -1;

  return projMat;
}
