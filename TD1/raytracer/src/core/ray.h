#pragma once

#include "common.h"
#include "vector.h"

class Shape;

class Ray {
public:
  Ray(const Point3f &o, const Vector3f &d, bool shadow = false)
      : origin(o), direction(d), recursionLevel(0) {}
  Ray() : recursionLevel(0) {}

  Point3f origin;
  Vector3f direction;

  Point3f at(float t) const { return origin + t * direction; }

  int recursionLevel; /// used as a stoping critera
};

class Hit {
public:
  /// Unoccluded distance along the ray
  float t;
  /// Shading normal
  Normal3f normal;
  /// Pointer to the associated shape
  const Shape *shape;

  Hit() : t(std::numeric_limits<float>::max()), shape(nullptr) {}

  bool foundIntersection() const {
    return t < std::numeric_limits<float>::max();
  }
};
