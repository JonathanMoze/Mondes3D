/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include "bsdf.h"
#include "texture.h"

/// Ideal mirror BRDF
class Mirror : public BSDF {
public:
  Mirror(const PropertyList &propList) {
    m_albedo = propList.getColor("albedo", Color3f(1.0f));

    m_type = MaterialType::Reflection;
  }

  Color3f albedo() const { return m_albedo; }

  Color3f eval(const Vector3f &wi, const Vector3f &wo,
               const Normal3f &n) const {
    /* Discrete BRDFs always evaluate to zero */
    return Color3f(0.f);
  }

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Mirror[\n"
                       "  albedo = %s\n"
                       "]",
                       m_albedo.toString());
  }

private:
  Color3f m_albedo;
};

REGISTER_CLASS(Mirror, "mirror");
