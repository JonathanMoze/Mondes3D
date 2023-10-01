#pragma once

#include "bsdf.h"
#include "common.h"
#include "object.h"
#include "ray.h"

class AreaLight;

/** represents a shape (geometry and BSDF)
 */
class Shape : public Object {
public:
  Shape() {}

  Shape(const PropertyList &) {}

  virtual void activate();

  /** Search the nearest intersection between the ray and the shape.
   * It must be implemented in the derived class. */
  virtual void intersect(const Ray &ray, Hit &hit) const {
    throw RTException(
        "Shape::intersect must be implemented in the derived class");
  }

  /** Return the axis-aligned bounding box of the geometry.
   * It must be implemented in the derived class. */
  virtual const BoundingBox3f &getBoundingBox() const {
    throw RTException("Shape::AABB must be implemented in the derived class");
  }

  virtual const BSDF *bsdf() const { return m_bsdf; }
  virtual void setBsdf(const BSDF *bsdf) { m_bsdf = bsdf; }

  /// Register a child object (e.g. a BSDF) with the shape
  virtual void addChild(Object *child);

  /// \brief Return the type of object provided by this instance
  EClassType getClassType() const { return EShape; }

  virtual std::string toString() const {
    throw RTException(
        "Shape::toString must be implemented in the derived class");
  }

protected:
  const BSDF *m_bsdf = nullptr;
};
