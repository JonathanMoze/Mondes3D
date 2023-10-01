#pragma once

#include "shape.h"

/**
 * @briefInfinte plane
 */
class Plane : public Shape {
public:
  Plane(const PropertyList &propList);

  virtual void intersect(const Ray &ray, Hit &hit) const;

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Plane[\n"
                       "  point = %s\n"
                       "  direction = %s\n"
                       "  BSDF = %s\n]",
                       ::toString(m_position),
                       ::toString(m_normal),
                       m_bsdf ? indent(m_bsdf->toString())
                              : std::string("null"));
  }

protected:
  Point3f  m_position;
  Vector3f m_normal;
};
