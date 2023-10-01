/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include "bsdf.h"
#include "texture.h"

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
  Dielectric(const PropertyList &propList) {
    /* Interior IOR (default: BK7 borosilicate optical glass) */
    m_intIOR = propList.getFloat("intIOR", 1.5046f);

    /* Exterior IOR (default: air) */
    m_extIOR = propList.getFloat("extIOR", 1.000277f);

    m_albedo = propList.getColor("albedo", Color3f(1.0f));

    m_type = MaterialType::ReflectionAndRefraction;
  }

  Color3f albedo() const { return m_albedo; }

  Color3f eval(const Vector3f &wi, const Vector3f &wo, const Normal3f &n) const {
    /* Discrete BRDFs always evaluate to zero */
    return Color3f(0.f);
  }

  float extIOR() const { return m_extIOR; }
  float intIOR() const { return m_intIOR; }

  std::string toString() const {
    return tfm::format("Dielectric[\n"
                       "  intIOR = %f,\n"
                       "  extIOR = %f,\n"
                       "  albedo = %s\n"
                       "]",
                       m_intIOR, m_extIOR, m_albedo.toString());
  }

private:
  float m_intIOR, m_extIOR;
  Color3f m_albedo;
};

REGISTER_CLASS(Dielectric, "dielectric");
