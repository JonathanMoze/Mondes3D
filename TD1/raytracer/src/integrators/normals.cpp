#include "bsdf.h"
#include "integrator.h"
#include "scene.h"

class NormalsIntegrator : public Integrator {
public:
  NormalsIntegrator(const PropertyList &props) { /* No parameters this time */
  }

  Color3f Li(const Scene *scene, const Ray &ray) const {
    Hit hit = Hit();
    scene->intersect(ray, hit);
    if(hit.shape != nullptr){
      Color3f col = Color3f(std::abs(hit.normal.x()), std::abs(hit.normal.y()), std::abs(hit.normal.z()));
      return col;
    } else {
      return scene->backgroundColor();
    }
  }

  std::string toString() const { return "NormalsIntegrator[]"; }
};

REGISTER_CLASS(NormalsIntegrator, "normals")
