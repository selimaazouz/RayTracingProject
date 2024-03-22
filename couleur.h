#ifndef COULEUR_H_INCLUDED
#define COULEUR_H_INCLUDED
#include <algorithm>
#include "vecteur3.h"
#include "rt.h"
#include <SFML/Graphics.hpp>
#include <vector>

#include <iostream>

class Couleur {
public:
    double r, g, b;

    Couleur(double rouge, double vert, double bleu) : r(rouge), g(vert), b(bleu) {}

    void restreindre() {
        r = std::clamp(r, 0.0, 1.0);
        g = std::clamp(g, 0.0, 1.0);
        b = std::clamp(b, 0.0, 1.0);
    }

    // Divise la couleur par le nombre d'échantillons
    void mettre_a_echelle(double echelle) {
        r *= echelle;
        g *= echelle;
        b *= echelle;
    }

    // Applique une correction gamma (élévation à la puissance 1/2.2)
    void corriger_gamma() {
        r = std::pow(r, 1.0 / 2.2);
        g = std::pow(g, 1.0 / 2.2);
        b = std::pow(b, 1.0 / 2.2);
    }
};

// Fonction write_color mise à jour pour utiliser la classe Couleur
void entrer_couleur(std::vector<sf::Uint8> &pix, Couleur coul_pix, int ech_par_pix, int lig, int col,
                 int larg_im) {
    coul_pix.restreindre();        // Assurer que les composantes de couleur sont dans la plage [0.0, 1.0]
    coul_pix.mettre_a_echelle(1.0 / ech_par_pix);  // Diviser par le nombre d'échantillons
    coul_pix.corriger_gamma();  // Appliquer la correction gamma

    // Convertir et écrire dans le vecteur de pixels
    pixels[(lig * larg_im+ col) * 4] = static_cast<sf::Uint8>(255 * coul_pix.r);
    pixels[(lig * larg_im + col) * 4 + 1] = static_cast<sf::Uint8>(255 * coul_pix.g);
    pixels[(lig * larg_im+ col) * 4 + 2] = static_cast<sf::Uint8>(255 * coul_pix.b);
    pixels[(lig * larg_im + col) * 4 + 3] = 255; // Canal alpha
}



#endif // COULEUR_H_INCLUDED
