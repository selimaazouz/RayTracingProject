#ifndef MATERIAU_H_INCLUDED
#define MATERIAU_H_INCLUDED

#include "rt.h"

#include "../include/tinyxml2.h"

struct EnregIntersect;

class materiau {
    public:
        virtual bool intercation(
            const rayon& r, const EnregIntersect& rec, couleur& attenuation, rayon& interactionR
        ) const = 0;
        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const {return nullptr;};
        static std::shared_ptr<materiau> materiau_from_xml(tinyxml2::XMLElement* pElement);
};

class LambertianMateriau : public materiau {
    public:
        LambertianMateriau(const couleur& a) : diffuseCouleur(a) {}

        LambertianMateriau(tinyxml2::XMLElement* pElement) {
            tinyxml2::XMLElement * couleur = pElement->FirstChildElement("couleur");

            diffuseCouleur = vecteur3(couleur->DoubleAttribute("r"), couleur->DoubleAttribute("g"), couleur->DoubleAttribute("b"));
        }

        virtual bool intercation(
            const rayon& r, const EnregIntersect& rec, couleur& attenuation, rayon& intercationR
        ) const override {
            auto intercation_direction = rec.surface_normal + vecteur_unitaire_aleatoire();

            // Catch degenerate intercation direction
            if (intercation_direction.proche_de_zero())
                intercation_direction = rec.surface_normal;

            intercationR = rayon(rec.p, intercation_direction, r.temps());
            attenuation = diffuseCouleur;
            return true;
        }

        tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const {
            tinyxml2::XMLElement * pElement = xmlDoc.NewElement("LambertianMateriau");

            tinyxml2::XMLElement * couleur = xmlDoc.NewElement("couleur");
            couleur->SetAttribute("r", diffuseCouleur.x());
            couleur->SetAttribute("g", diffuseCouleur.y());
            couleur->SetAttribute("b", diffuseCouleur.z());

            pElement->InsertEndChild(couleur);

            return pElement;
        }

    public:
        couleur diffuseCouleur;
};

class MetalMateriau : public materiau {
    public:
        MetalMateriau(const couleur& a, double f) : diffuseCouleur(a), reflectionfuzz(f < 1 ? f : 1) {}

        MetalMateriau(tinyxml2::XMLElement* pElement) {
            reflectionfuzz = pElement->DoubleAttribute("reflectionfuzz");
            tinyxml2::XMLElement * couleur = pElement->FirstChildElement("couleur");
            diffuseCouleur = vecteur3(couleur->DoubleAttribute("r"), couleur->DoubleAttribute("g"), couleur->DoubleAttribute("b"));
        }

        virtual bool intercation(
            const rayon& r, const EnregIntersect& rec, couleur& attenuation, rayon& intercationR
        ) const override {
            vecteur3 reflected = reflect(vecteur_unitaire(r.direction()), rec.surface_normal);
            intercationR = rayon(rec.p, reflected + reflectionfuzz*random_in_unit_sphere(), r.temps());
            attenuation = diffuseCouleur;
            return (produit_scalaire(intercationR.direction(), rec.surface_normal) > 0);
        }

        tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const {
            tinyxml2::XMLElement * pElement = xmlDoc.NewElement("MetalMateriau");

            tinyxml2::XMLElement * couleur = xmlDoc.NewElement("couleur");
            couleur->SetAttribute("r", diffuseCouleur.x());
            couleur->SetAttribute("g", diffuseCouleur.y());
            couleur->SetAttribute("b", diffuseCouleur.z());

            pElement->InsertEndChild(couleur);

            pElement->SetAttribute("reflectionfuzz", reflectionfuzz);

            return pElement;
        }

    public:
        couleur diffuseCouleur;
        double reflectionfuzz;
};

class DielectricMateriau : public materiau {
    public:
        DielectricMateriau(double indexrefraction) : indexRefraction(indexrefraction) {}

        DielectricMateriau(tinyxml2::XMLElement* pElement) {
            indexRefraction = pElement->DoubleAttribute("Ir");
        }

        virtual bool intercation(
            const rayon& r, const EnregIntersect& rec, couleur& attenuation, rayon& intercationR
        ) const override {
            attenuation = couleur(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vecteur3 unit_direction = vecteur_unitaire(r.direction());
            double cos = fmin(produit_scalaire(-unit_direction, rec.surface_normal), 1.0);
            double sin = sqrt(1.0 - cos*cos);

            bool cannot_refract = refraction_ratio * sin > 1.0;
            vecteur3 direction;

            if (cannot_refract || reflectance(cos, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.surface_normal);
            else
                direction = refract(unit_direction, rec.surface_normal, refraction_ratio);

            intercationR = rayon(rec.p, direction, r.temps());
            return true;
        }

        tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const {
            tinyxml2::XMLElement * pElement = xmlDoc.NewElement("DielectricMateriau");

            pElement->SetAttribute("indexRefraction ", indexRefraction );

            return pElement;
        }

    public:
        double indexRefraction ; // Index of Refraction

	private:
		static double reflectance(double cosine, double ref_idx) {
			// Use Schlick's approximation for reflectance.
			auto r0 = (1-ref_idx) / (1+ref_idx);
			r0 = r0*r0;
			return r0 + (1-r0)*pow((1 - cosine),5);
		}
};

std::shared_ptr<materiau> materiau::materiau_from_xml(tinyxml2::XMLElement* pElement) {
    tinyxml2::XMLElement* matElement = pElement->FirstChildElement();
    if (strcmp(matElement->Name(), "LambertianMateriau") == 0) {
        return std::make_shared<LambertianMateriau>(matElement);
    }
    else if (strcmp(matElement->Name(), "MetalMateriau") == 0) {
        return std::make_shared<MetalMateriau>(matElement);
    }
    else if (strcmp(matElement->Name(), "DielectricMateriau") == 0) {
        return std::make_shared<DielectricMateriau>(matElement);
    }
    else {
        throw std::invalid_argument("materiau " + std::string(matElement->Name()) + " isn't defined");
    }
}

#endif // MATERIAU_H_INCLUDED
