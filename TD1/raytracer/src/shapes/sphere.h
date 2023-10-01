#pragma once

#include "shape.h"

#include <Eigen/Core>

/** Represents a sphere
 */
class Sphere : public Shape {
public:
  Sphere(float radius);
  Sphere(const PropertyList &propList);

  virtual void intersect(const Ray &ray, Hit &hit) const;

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Sphere[\n"
                       "  center = %s,\n"
                       "  radius = %f\n"
                       "  BSDF = %s,\n]"
                       "]",
                       ::toString(m_center), m_radius,
                       m_bsdf ? indent(m_bsdf->toString())
                              : std::string("null"));
  }

protected:
  Point3f m_center;
  float m_radius;
};
