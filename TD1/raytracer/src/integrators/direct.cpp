#include "bsdf.h"
#include "integrator.h"
#include "scene.h"

class DirectIntegrator : public Integrator {
public:
  DirectIntegrator(const PropertyList &props) { /* No parameters this time */
  }

  Color3f Li(const Scene *scene, const Ray &ray) const {
    Hit hit = Hit();
    scene->intersect(ray, hit);
    if(hit.shape != nullptr){
      Normal3f norm = hit.normal;
      Point3f x = ray.at(hit.t);
      Vector3f wo = ray.direction;
      Color3f col = Color3f();
      for(auto light: scene->lightList()){     

        Vector3f wi = Vector3f();
        
        float dist = 0.f;
        Color3f intensity = light->intensity(x, wi, dist);

        
        Ray lightRay = Ray(x, wi);
        Point3f origin = lightRay.at(0.0001f);
        lightRay.origin = origin;
        Hit lightHit = Hit();
        scene->intersect(lightRay, lightHit);
        if(lightHit.t<dist){
          continue;
        } else {
          col += (hit.shape->bsdf()->eval(wi, wo, hit.normal))*std::max(wi.dot(norm), 0.f)*intensity;
        }
        
        
      }

      return col;
    } else {
      return scene->backgroundColor();
    }
  }

  std::string toString() const { return "DirectIntegrator[]"; }
};

REGISTER_CLASS(DirectIntegrator, "direct")
