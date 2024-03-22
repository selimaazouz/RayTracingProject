#ifndef MOBILE_SPHERE_H_INCLUDED
#define MOBILE_SPHERE_H_INCLUDED

#include "rt.h"
#include "BoundingBox.h"

#include "ObjectHit.h"

#include "../include/tinyxml2.h"

#include "materiau.h"

class Mobile_Sphere : public Object{
    public:
         point center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<materiau> materiau_ptr;
        Mobile_Sphere() {}
        Mobile_Sphere(
            point c0, point c1, double t0, double t1, double r, shared_ptr<materiau> m)
            : center0(c0), center1(c1), time0(t0), t1(t1), radius(r), materiau_ptr(m)
        {};
        Mobile_Sphere(tinyxml2::XMLElement* elt);

        virtual bool intersect(
            const rayon& r, double t_min, double t_max, EnregIntersect& record) const override;

		virtual bool bounding_box(
            double t0, double t1, BoundingBox& ob) const override;

        point center(double t) const;

        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;


};

Mobile_Sphere::Mobile_Sphere(tinyxml2::XMLElement* elt) {
    radius = elt->DoubleAttribute("Radius");
    time0 = elt->DoubleAttribute("Time0");
    time1 = elt->DoubleAttribute("Time1");

    tinyxml2::XMLElement* center0_xml = elt->FirstChildElement("Center0");
    center0 = point(center0_xml->DoubleAttribute("x"), center0_xml->DoubleAttribute("y"), center0_xml->DoubleAttribute("z"));

    tinyxml2::XMLElement* center1_xml = elt->FirstChildElement("Center1");
    center1 = point(center1_xml->DoubleAttribute("x"), center1_xml->DoubleAttribute("y"), center1_xml->DoubleAttribute("z"));

    materiau_ptr = materiau::materiau_from_xml(elt->FirstChildElement("materiau"));
}

point Mobile_Sphere::center(double t) const {
    return center0 + ((t - time0) / (time1 - time0))*(center1 - center0);
}

bool Mobile_Sphere::intersect(const rayon& r, double t_min, double t_max, EnregIntersect& record) const {
    vecteur3 oc = r.origin() - center(r.temps());
    auto a = r.direction().norme2();
    auto half = produit_scalaire(oc, r.direction());
    auto c = oc.norme2() - radius*radius;

    auto discr = half*half - a*c;
    if (discr < 0) return false;
    auto sqrtd = sqrt(discr);


    auto root = (-half - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    record.t = root;
    record.p = r.at(record.t);
    auto surface_normal_at_intersection  = (record.p - center(r.temps())) / radius;
    record.compute_face_normal(r, surface_normal_at_intersection );
    record.materiau_ptr = materiau_ptr;

    return true;
}

bool Mobile_Sphere::bounding_box(double t0, double t1, BoundingBox& ob) const {
    BoundingBox b0(
        center(t0) - vecteur3(radius, radius, radius),
        center(t0) + vecteur3(radius, radius, radius));
    BoundingBox b1(
        center(t1) - vecteur3(radius, radius, radius),
        center(t1) + vecteur3(radius, radius, radius));
    ob = creer_surrounding_box(b0, b1);
    return true;
}

tinyxml2::XMLElement* Mobile_Sphere::to_xml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement * elt = xmlDoc.NewElement("Mobile_Sphere");

    elt->SetAttribute("Radius", radius);
    elt->SetAttribute("Time0", time0);
    elt->SetAttribute("Time1", time1);

    tinyxml2::XMLElement* center0_xml = xmlDoc.NewElement("Center0");

    center0_xml->SetAttribute("x", center0.x());
    center0_xml->SetAttribute("y", center0.y());
    center0_xml->SetAttribute("z", center0.z());

    elt->InsertEndChild(center0_xml);

    tinyxml2::XMLElement* center1_xml = xmlDoc.NewElement("Center1");

    center1_xml->SetAttribute("x", center1.x());
    center1_xml->SetAttribute("y", center1.y());
    center1_xml->SetAttribute("z", center1.z());

    elt->InsertEndChild(center1_xml);

    tinyxml2::XMLElement* materiau_xml = xmlDoc.NewElement("materiau");
    tinyxml2::XMLElement* materiauElement = materiau_ptr->to_xml(xmlDoc);

    materiau_xml->InsertEndChild(materiauElement);

    elt->InsertEndChild(materiau_xml);

    return elt;
}

#endif // MOBILE_SPHERE_H_INCLUDED
