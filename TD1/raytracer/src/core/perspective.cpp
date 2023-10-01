/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <Eigen/Geometry>

#include "camera.h"

/**
 * \brief Perspective camera with an infinite depth of field
 *
 * This class implements a simple perspective camera model. It uses an
 * infinitesimally small aperture, creating an infinite depth of field.
 */
class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const PropertyList &propList) {
    /* Width and height in pixels. Default: 720p */
    m_outputSize.x() = propList.getInteger("width", 1280);
    m_outputSize.y() = propList.getInteger("height", 720);

    /* Specifies an optional camera-to-world transformation. Default: none */
    m_cameraToWorld = propList.getTransform("toWorld", Transform());

    /* Horizontal field of view in degrees */
    m_fov = propList.getFloat("fov", 30.0f);

    /* Near and far clipping planes in world-space units */
    m_nearClip = propList.getFloat("nearClip", 1e-4f);
    m_farClip = propList.getFloat("farClip", 1e4f);
  }

  void activate() {
    float aspect = m_outputSize.x() / (float)m_outputSize.y();

    float tanfovy2 = std::tan(degToRad(m_fov * 0.5f));
    m_camX = m_cameraToWorld.getMatrix().col(0).head<3>() * tanfovy2 *
             m_nearClip * aspect;
    m_camY =
        m_cameraToWorld.getMatrix().col(1).head<3>() * tanfovy2 * m_nearClip;
    m_camF = -m_cameraToWorld.getMatrix().col(2).head<3>() * m_nearClip;
    m_center = m_cameraToWorld.getMatrix().col(3).head<3>();
  }

  void sampleRay(Ray &ray, const Point2f &samplePosition) const {

    float xR = ((samplePosition.x()) -(outputSize().x()/2.f)) * (2.f/outputSize().x());
    float yR = -((samplePosition.y()) -(outputSize().y()/2.f)) * (2.f/outputSize().y());

    Vector3f vect = m_camF;
    vect += xR*m_camX;
    vect += yR*m_camY;
    vect.normalize();

    ray.origin = m_center;
    ray.direction = vect;

  }

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("PerspectiveCamera[\n"
                       "  cameraToWorld = %s,\n"
                       "  outputSize = %s,\n"
                       "  fov = %f,\n"
                       "  clip = [%f, %f],\n"
                       "]",
                       indent(m_cameraToWorld.toString(), 18),
                       m_outputSize.toString(), m_fov, m_nearClip, m_farClip);
  }

private:
  Transform m_cameraToWorld;
  Vector3f m_camF, m_camX, m_camY, m_center;
  float m_fov;
  float m_nearClip;
  float m_farClip;
};

REGISTER_CLASS(PerspectiveCamera, "perspective");
