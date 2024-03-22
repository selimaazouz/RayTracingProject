#ifndef MOTEURDERENDU_H_INCLUDED
#define MOTEURDERENDU_H_INCLUDED
#include <SFML/Graphics.hpp>
#include <ostream>
#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include "ObjectList.h"
#include "couleur.h"
#include "vecteur3.h"
#include "rayon.h"
#include "rt.h"
#include "camera.h"
#include "materiau.h"

class MoteurRendu {
private:
    sf::Texture texture;

    int largeur_img;
    int hauteur_img;
    std::vector<sf::Uint8> pixels;
    int echantillons_par_pixel;
    double rapport_aspect;
    int profondeur_max;
    ObjectList monde;
    camera cam;
    bool image_pret=false;

    // Variables pour activer la barre de progression
    bool en_travail = false;
    int lignes_restantes = 0;
    std::chrono::time_point<std::chrono::steady_clock> temps_debut;

public:
    MoteurRendu();

    MoteurRendu(const MoteurRendu&);

    MoteurRendu(unsigned int largeur_image, unsigned int hauteur_image,
               int echantillons_par_pixel = 50,
               int profondeur_max = 20);

    MoteurRendu(const char* nom_fichier_xml);

    void sauvegarderDocumentXml(const char* nom_fichier) const;

    void creerImage();

    void rendreImage();

    void rendreImage(sf::Texture&);

    void rendreImage(int nouvelle_largeur_img, int nouvelle_hauteur_img);

    void rendreImage(sf::Texture&, int nouvelle_largeur_img, int nouvelle_hauteur_img);

    void definirEchantillonsParPixel(int valeur) {
        echantillons_par_pixel = valeur;
    }

    void definirProfondeurMax(int valeur) {
        profondeur_max = valeur;
    }

    void definirRapportAspect(double valeur) {
        rapport_aspect = valeur;
        hauteur_img = static_cast<int>(largeur_img / rapport_aspect);
    }

    void definirLargeurImage(int valeur) {
        largeur_img = valeur;
        rapport_aspect = static_cast<double>(largeur_img / hauteur_img);
    }

    void definirHauteurImage(int valeur) {
        hauteur_img = valeur;
        rapport_aspect = static_cast<double>(largeur_img / hauteur_img);
    }

    void commencerTravail() {
        en_travail = true;
    }

    bool aImagePret() { return image_pret; }
    sf::Texture& obtenirTexture() { return texture; }
    int obtenirLargeurImage() { return largeur_img; }
    int obtenirHauteurImage() { return hauteur_img; }

    // Méthodes utiles pour la barre de progression
    bool estEnTravail() { return en_travail; }
    std::chrono::time_point<std::chrono::steady_clock> obtenirTempsDebutTravail() { return temps_debut; }
    int obtenirLignesRestantes() { return lignes_restantes; }

    void definircamera( point regarde_de,
        point regarde_vers,
        vecteur   vertical,
        double champ_de_vision, // angle de champ vertical en degrés
        double ouverture_diaphragme,
        double distance_de_focus,double temps0 = 0,
        double temps1 = 0) {
            cam = camera(regarde_de, regarde_vers, vertical, champ_de_vision, rapport_aspect, ouverture_diaphragme,
                distance_de_focus, temps0, temps1);
        }

    void ajouterAuMonde(std::shared_ptr<Object> objet) {
        monde.ajouter(objet);
    }
};

MoteurRendu::MoteurRendu() : largeur_img(480), hauteur_img(400), pixels(4*largeur_img*hauteur_img),
    echantillons_par_pixel(100), profondeur_max(50) {
        texture = sf::Texture();
        texture.create(largeur_img, hauteur_img);
        rapport_aspect = (double) largeur_img/hauteur_img;
        point regarde_de(13,2,3);
        point regarde_vers(0,0,0);

        vecteur vertical(0,1,0);

        auto distance_de_focus = 10.0;
        auto ouverture_diaphragme = 0.1;

        cam = camera(regarde_de, regarde_vers, vertical, 20.0, rapport_aspect, ouverture_diaphragme, distance_de_focus, 0.0, 1.0);
        monde = scene_aleatoire();
    }

MoteurRendu::MoteurRendu(unsigned int largeur_image, unsigned int hauteur_image,
               int echantillons_par_pixel,
               int profondeur_max) :
    largeur_img(largeur_image), hauteur_img(hauteur_image), pixels(largeur_img*hauteur_img*4),
    echantillons_par_pixel(echantillons_par_pixel), rapport_aspect(largeur_img / hauteur_img), profondeur_max(profondeur_max), monde(), cam()  {

        texture = sf::Texture();
        texture.create(largeur_img, hauteur_img);
    }

MoteurRendu::MoteurRendu(const char* nom_fichier) {
    tinyxml2::XMLDocument xmlDoc;

    tinyxml2::XMLError eResult = xmlDoc.LoadFile(nom_fichier);
    //XMLCheckResult(eResult);

    tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
    if (pRoot == nullptr) throw std::invalid_argument("Le fichier ne contient pas d'élément racine");

    tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("MoteurRendu");
    if (pElement == nullptr) throw std::invalid_argument("Le fichier ne contient pas d'élément moteur de rendu");

    largeur_img = pElement->IntAttribute("LargeurImg");
    hauteur_img = pElement->IntAttribute("HauteurImg");
    echantillons_par_pixel = pElement->IntAttribute("EchantillonsParPixel");
    rapport_aspect = pElement->DoubleAttribute("RapportAspect");
    profondeur_max = pElement->IntAttribute("ProfondeurMax");

    pixels = std::vector<sf::Uint8>(4*largeur_img*hauteur_img);
    texture = sf::Texture();
    texture.create(largeur_img, hauteur_img);

    tinyxml2::XMLElement * pElementcamera = pElement->FirstChildElement("camera");
    if (pElementcamera == nullptr) throw std::invalid_argument("Le fichier ne contient pas d'élément camera");

    cam = camera(pElementcamera);

    tinyxml2::XMLElement * pElementListe = pRoot->FirstChildElement("Liste");
    if (pElementListe == nullptr) throw std::invalid_argument("Le fichier ne contient pas d'élément liste");

    monde = ObjectList(pElementListe);
}

void MoteurRendu::sauvegarderDocumentXml(const char* nom_fichier) const{
    tinyxml2::XMLDocument xmlDoc;

    tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Racine");
    xmlDoc.InsertFirstChild(pRoot);

    tinyxml2::XMLElement * pElement = xmlDoc.NewElement("MoteurRendu");

    pElement->SetAttribute("LargeurImg", largeur_img);
    pElement->SetAttribute("HauteurImg", hauteur_img);
    pElement->SetAttribute("EchantillonsParPixel", echantillons_par_pixel);
    pElement->SetAttribute("RapportAspect", rapport_aspect);
    pElement->SetAttribute("ProfondeurMax", profondeur_max);

    pElement->InsertEndChild(cam.to_xml(xmlDoc));
    pRoot->InsertEndChild(pElement);

    pRoot->InsertEndChild(monde.to_xml(xmlDoc));

    xmlDoc.SaveFile(nom_fichier);
}

// Retourne la couleur d'un rayon
couleur couleur_rayon(const rayon& r, const ObjectList& monde, int profondeur) {
    EnregIntersect rec;

    // Si nous avons dépassé la limite de rebonds du rayon, plus de lumière n'est collectée.
    if (profondeur <= 0)
        return couleur(0,0,0);

    if (monde.impact(r, 0.001, infini, rec)) {
        rayon interactionR;
        couleur attenuation;

        if (rec.materiau_ptr->interaction(r, rec, attenuation, interactionR))
            return attenuation * couleur_rayon(interactionR, monde, profondeur-1);
        return couleur(0,0,0);
    }
    vecteur direction_unite = vecteur_unitaire(r.direction());
    auto t = 0.5*(direction_unite.y() + 1.0);
    return (1.0-t)*couleur(1.0, 1.0, 1.0) + t*couleur(0.5, 0.7, 1.0);
}

void MoteurRendu::creerImage()
{
    if (en_travail) {
        // Rendu
        pixels.clear();
        start_time = std::chrono::steady_clock::now();
        for (int j = hauteur_img-1; j >= 0; --j) {
            lignes_restantes = j;
            #pragma omp parallel for schedule(dynamic, 10)
            for (int i = 0; i < largeur_img; ++i) {
                couleur couleur_pixel(0, 0, 0);
                for (int s = 0; s < echantillons_par_pixel; ++s) {
                    auto u = (i + random_double()) / (largeur_img-1);
                    auto v = (j + random_double()) / (hauteur_img-1);
                    rayon r = cam.obtenirrayon(u, v);
                    couleur_pixel += couleur_rayon(r, monde, profondeur_max);
                }
                ecrire_couleur(pixels, couleur_pixel, echantillons_par_pixel, (hauteur_img-1) - j, i, largeur_img);
            }
        }
        en_travail = false;
        image_pret = true;
    }
}

void MoteurRendu::rendreImage() {
    creerImage();
    texture.create(largeur_img, hauteur_img);
    texture.update(pixels.data());
}

void MoteurRendu::rendreImage(sf::Texture& nouvelle_texture) {
    texture = nouvelle_texture;
    rendreImage();
}

void MoteurRendu::rendreImage(int nouvelle_largeur_img, int nouvelle_hauteur_img) {
    if (hauteur_img != nouvelle_hauteur_img || largeur_img != nouvelle_largeur_img) {
        hauteur_img = nouvelle_hauteur_img;
        largeur_img = nouvelle_largeur_img;

        pixels.resize(hauteur_img*largeur_img*4);
    }

    rendreImage();
}

void MoteurRendu::rendreImage(sf::Texture& nouvelle_texture, int nouvelle_largeur_img, int nouvelle_hauteur_img) {
    if (hauteur_img != nouvelle_hauteur_img || largeur_img != nouvelle_largeur_img) {
        hauteur_img = nouvelle_hauteur_img;
        largeur_img = nouvelle_largeur_img;

        pixels.resize(hauteur_img*largeur_img*4);
    }
    texture = nouvelle_texture;

    rendreImage();
}







#endif // MOTEURDERENDU_H_INCLUDED
