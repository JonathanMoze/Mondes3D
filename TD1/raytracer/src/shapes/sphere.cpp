#include "sphere.h"

Sphere::Sphere(float radius) : m_radius(radius) {}

Sphere::Sphere(const PropertyList &propList) {
  m_radius = propList.getFloat("radius", 1.f);
  m_center = propList.getPoint("center",Point3f(0,0,0));
}

void Sphere::intersect(const Ray &ray, Hit &hit) const {
  /// TODO: compute ray-sphere intersection
  float a = 1.f;
  float b = 2.f*(ray.direction.dot(ray.origin-m_center));
  float c = ((ray.origin-m_center).dot(ray.origin-m_center) - std::pow(m_radius, 2));


  float discr = std::pow(b,2)-(4.f*a*c);

  if(discr < 0)
    return;

  hit.shape = this;
  if(discr == 0){
    hit.t = (-b)/2*a;
    
  } else {
    float r1 = ((-b)-std::sqrt(discr))/2;
    float r2 = ((-b)+std::sqrt(discr))/2;
    hit.t = (r1 < r2)? r1 : r2;
  }

  const Point3f inter = ray.at(hit.t);
  Vector3f norm = Vector3f(inter.x()-m_center.x(), inter.y()-m_center.y(), inter.z()-m_center.z());
  norm.normalize();

  hit.normal = norm;



}

REGISTER_CLASS(Sphere, "sphere")
