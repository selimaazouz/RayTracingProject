#ifndef SPHERE_H_INCLUDED
#define SPHERE_H_INCLUDED

#include "ObjectHit.h"
#include "vecteur3.h"

#include "../include/tinyxml2.h"

#include "materiau.h"
class SphereObject :public Object {
public:
    point center;
    double radius;
    std::shared_ptr<Materiau> Materiau;
    SphereObject() {}
    SphereObject(point center, double radius) : center(center), radius(radius) {};
    SphereObject(point center, double radius, std::shared_ptr<Materiau> Materiau)
        : center(center), radius(radius), materiau(Materiau) {};
    SphereObject(tinyxml2::XMLElement* element);

    virtual bool intersect(
        const rayon& r, double t_min, double t_max, EnregIntersect& record) const override;

    virtual bool bounding_Box(double time0, double time1, boundingBox& ob) const override;

    virtual tinyxml2::XMLElement* toXml(tinyxml2::XMLDocument& xmlDoc) const override;


};

SphereObject::SphereObject(tinyxml2::XMLElement* element) {
    radius = element->DoubleAttribute("Radius");

    tinyxml2::XMLElement* centerXml = element->FirstChildElement("Center");
    center = point(centerXml->DoubleAttribute("x"), centerXml->DoubleAttribute("y"), centerXml->DoubleAttribute("z"));

    materiau = Materiau::createFromXml(element->FirstChildElement("Materiau"));
}

bool SphereObject::intersect(const rayon& r, double t_min, double t_max, EnregIntersect& record) const {
    Vecteur3 oc = r.origin() - center;
    auto a = r.direction().norme2();
    auto half = produit_scalaire(oc, r.direction());
    auto c = oc.norme2() - radius * radius;

    auto discr = half * half - a * c;
    if (discr < 0) return false;
    auto sqrtD = sqrt(discr);

    auto root = (-half - sqrtD) / a;
    if (root < t_min || t_max < root) {
        root = (-half + sqrtD) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    record.t = root;
    record.point = r.at(record.t);
    vecteur3 surface_normal_at_intersection = (record.point - center) / radius;
    record.compute_face_normal(r, surface_normal_at_intersection);
    record.materiau_ptr = materiau;

    return true;
}

bool SphereObject::bounding_Box(double time0, double time1, BoundingBox& ob) const {
    ob= BoundingBox(
        center - vecteur3(radius, radius, radius),
        center + vecteur3(radius, radius, radius));
    return true;
}

tinyxml2::XMLElement* SphereObject::toXml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement* element = xmlDoc.NewElement("Sphere");

    element->SetAttribute("Radius", radius);

    tinyxml2::XMLElement* centerXml = xmlDoc.NewElement("Center");

    centerXml->SetAttribute("x", center.x());
    centerXml->SetAttribute("y", center.y());
    centerXml->SetAttribute("z", center.z());

    element->InsertEndChild(centerXml);

    tinyxml2::XMLElement* MateriauXml = xmlDoc.NewElement("Materiau");
    tinyxml2::XMLElement* MateriauElement = materiau->toXml(xmlDoc);

    MateriauXml->InsertEndChild(MateriauElement);

    element->InsertEndChild(MateriauXml);

    return element;
}



#endif // SPHERE_H_INCLUDED
