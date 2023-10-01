#pragma once

#include "bitmap.h"
#include "object.h"

class Light : public Object {
public:
  Light() {}

  /** Returns the intensity emitted at point \param x
   *  and computes the incident direction \param [out] wi
   *  and distance \param [out] dist to the light source
   */
  virtual Color3f intensity(const Point3f &x, Vector3f &wi,
                            float &dist) const = 0;

  EClassType getClassType() const { return ELight; }
};

typedef std::vector<Light *> LightList;
