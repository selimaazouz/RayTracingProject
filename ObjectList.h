#ifndef OBJECTLIST_H_INCLUDED
#define OBJECTLIST_H_INCLUDED
#include "ObjectHit.h"
#include "sphere.h"
#include "Mobile_Sphere.h"
#include "BoundingBox.h"

#include <memory>
#include <vector>
#include <iostream>
#include <cstring>

#include "../include/tinyxml2.h"

#include "materiau.h"

using std::shared_ptr;
using std::make_shared;

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

class ObjectList :
public Object {
public:
    ObjectList() {}
    ObjectList(shared_ptr<Object> obj) { add(obj); }
    ObjectList(const char* xml_filename);
    ObjectList(tinyxml2::XMLElement * element);

    void clear() { objects.clear(); }
    void add(shared_ptr<Object> obj) { objects.push_back(obj); }

    virtual bool intersect(const rayon& r, double t_min, double t_max, EnregIntersect& record) const override;

	virtual bool bounding_box(double time0, double time1, BoundingBox& ob) const override;

    virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;

    void saveXmlDocument(char* filename);

public:
    std::vector<shared_ptr<Object>> objects;
};

bool ObjectList::intersect(const rayon& r, double t_min, double t_max, EnregIntersect& record) const {
    EnregIntersect temp_rec;
   bool object_was_hit = false;
auto closest_hit_distance = t_max;

   for (const auto& obj : objects) {
    if (obj->intersect(r, t_min, closest_hit_distance, temp_intersection)) {
        object_was_hit = true;
        closest_hit_distance = temp_intersection.t;
        record = temp_intersection;
    }
}

return object_was_hit;
}

bool ObjectList::bounding_box(double time0, double time1, BoundingBox& ob) const {
     if (objects.empty()) return false;

    BoundingBox temp_b;
    bool fb= true;

    for (const auto& obj : objects) {
        if (!obj->bounding_box(time0, time1, temp_b)) return false;
        ob = fb ? temp_b : creer_surrounding_box(ob, temp_b);
        fb = false;
    }

    return true;
}

tinyxml2::XMLElement* ObjectList::to_xml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement * element = xmlDoc.NewElement("ObjectList");

    for (auto & item : objects) {
        tinyxml2::XMLElement * listElement = item->to_xml(xmlDoc);
        element->InsertEndChild(listElement);
    }

    return element;
}

ObjectList::ObjectList(tinyxml2::XMLElement * element) {
    tinyxml2::XMLElement * listElement = element->FirstChildElement();
    while (listElement != nullptr) {
        if (strcmp(listElement->Name(), "Sphere") == 0) {
            objects.push_back(make_shared<Sphere>(listElement));
        }
        else if (strcmp(listElement->Name(), "Moving_Sphere") == 0) {
            objects.push_back(make_shared<MovingSphere>(listElement));
        }
        else {
            throw std::invalid_argument("Object not defined or list inside list");
        }

        listElement = listElement->NextSiblingElement();
    }
}

ObjectList generate_random_scene() {
    ObjectList scene;

    auto baseMaterial = make_shared<LambertianMaterial>(couleur(0.5, 0.5, 0.5));
    scene.add(make_shared<SphereObject>(point(0,-1000,0), 1000, baseMaterial));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choice_material = random_double();
            point center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Materiau> sphere_materiau;

                if (choice_material < 0.33) {
                    auto diffuseCouleur = couleur::random() * couleur::random();
                    sphere_material = make_shared<LambertianMaterial>(diffuseCouleur);
                    auto center2 = center + Vec3(0, random_double(0, .5), 0);
                    scene.add(make_shared<MovingSphereObject>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choice_material < 0.66) {
                    auto diffuseCouleur = couleur::random(0.5, 1);
                    auto reflectionfuzz = random_double(0, 0.5);
                    sphere_materiau = make_shared<MetalMateriau>(diffuseCouleur, fuzz);
                    scene.add(make_shared<SphereObject>(center, 0.2, sphere_materiau));
                } else {
                    sphere_materiau= make_shared<DielectricMateriau>(1.5);
                    scene.add(make_shared<SphereObject>(center, 0.2, sphere_materiau));
                }
            }
        }
    }

    auto mat1 = make_shared<DielectricMateriau>(1.5);
    scene.add(make_shared<SphereObject>(point(0, 1, 0), 1.0, mat1));

    auto mat2 = make_shared<LambertianMateriau>(couleur(0.4, 0.2, 0.1));
    scene.add(make_shared<SphereObject>(point(-4, 1, 0), 1.0, mat2));

    auto mat3 = make_shared<MetalMateriau>(couleur(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<SphereObject>(point(4, 1, 0), 1.0, mat3));

    return scene;
}



#endif // OBJECTLIST_H_INCLUDED
