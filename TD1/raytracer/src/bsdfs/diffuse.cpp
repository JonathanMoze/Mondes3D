/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include "bsdf.h"

/**
 * \brief Diffuse / Lambertian BRDF model
 */
class Diffuse : public BSDF {
public:
  Diffuse(const PropertyList &propList) {
    m_albedo = propList.getColor("albedo", Color3f(0.5f));

    m_type = MaterialType::DiffuseAndGlossy;
  }
  
  Color3f albedo() const { return m_albedo; }

  /// Evaluate the BRDF model
  Color3f eval(const Vector3f &wi, const Vector3f &wo, const Normal3f &n) const {
    
    return m_albedo;
  }

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Diffuse[\n"
                       "  albedo = %s\n"
                       "]",
                       m_albedo.toString());
  }

  EClassType getClassType() const { return EBSDF; }

private:
  Color3f m_albedo;
};

REGISTER_CLASS(Diffuse, "diffuse");
