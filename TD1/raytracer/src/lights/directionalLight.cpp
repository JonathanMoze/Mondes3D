#include "light.h"

class DirectionalLight : public Light {
public:
  DirectionalLight(const PropertyList &propList) {
    m_intensity = propList.getColor("radiance", Color3f(1.f));
    m_direction =
        propList.getVector("direction", Point3f::UnitX()).normalized();
  }

  Color3f intensity(const Point3f &x, Vector3f &wi, float &dist) const {
    wi = -m_direction;
    dist = std::numeric_limits<float>::max();
    return m_intensity;
  }

  std::string toString() const {
    return tfm::format("DirectionalLight[\n"
                       "  intensity = %s\n"
                       "  direction = %s\n"
                       "]",
                       m_intensity.toString(), ::toString(m_direction));
  }

protected:
  Color3f m_intensity;
  Vector3f m_direction;
};

REGISTER_CLASS(DirectionalLight, "directionalLight")
