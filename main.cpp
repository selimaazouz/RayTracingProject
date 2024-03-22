#include "Fenetre.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <array>
#include <cstring>
#include <X11/Xlib.h>
#include "MoteurDeRendu.h"
#include "InterfaceTerminal.h"

auto rapport_aspect = 3.0 / 2.0;
unsigned int largeur_image = 400;
unsigned int hauteur_image = static_cast<unsigned int>(largeur_image / rapport_aspect);

int principal(int argc, char *argv[])
{
    char fichier_origine[40], fichier_dest[40], fichier_image_dest[40];
    bool a_fichier_origine = false, a_fichier_dest=false, sauvegarder_image=false;

    if (argc > 1) {
        for (auto i = 1; i < argc; i++) {
            if (strncmp(argv[i], "--origine=", 10) == 0) {
                strcpy(fichier_origine, argv[i]+10);
                a_fichier_origine = true;
                // std::cout << "Geting file from " << argv[i]+10 << std::endl;
            }
            else if (strncmp(argv[i], "--dest=", 7) == 0) {
                strcpy(fichier_dest, argv[i]+7);
                a_fichier_dest=true;
            }
            else if (strncmp(argv[i], "--sauvegarder-image=", 20) == 0) {
                strcpy(fichier_image_dest, argv[i]+20);
                sauvegarder_image=true;
            }
        }
    }

    XInitThreads();

    // window.setActive(false);

    //Moteur rtMoteur(texture, largeur_image, hauteur_image);
    MoteurRendu rtMoteur;
    if (a_fichier_origine) {
        rtMoteur = MoteurRendu(fichier_origine);
    }
    else {
        rtMoteur = MoteurRendu();
    }

    sf::Sprite sprite(rtMoteur.getTexture());

    sf::VideoMode mode_video(rtMoteur.obtenirLargeurImage(), rtMoteur.obtenirHauteurImage());
    sf::RenderWindow fenetre(mode_video, "Moteur de Tracé de Rayons", sf::Style::Default & (~sf::Style::Close));
    fenetre.setVisible(false);

    InterfaceTerminal terminal(fenetre, rtMoteur);
    terminal.initialiser();


    // exécuter le programme tant que la fenêtre est ouverte
    while (!terminal.estTempsDeFermer())
    {
        // vérifier tous les événements de la fenêtre qui ont été déclenchés depuis la dernière itération de la boucle
        sf::Event evenement;
        while (fenetre.pollEvent(evenement))
        {
            // événement "demande de fermeture" : masquer la fenêtre
            if (evenement.type == sf::Event::Closed) {
                // fenetre.close();
                fenetre.setVisible(false);
            }
        }

        if (rtMoteur.estEnTravail()) {
            fenetre.clear();
            fenetre.setVisible(false);
            rtMoteur.rendreImage();
            fenetre.setVisible(true);
            fenetre.draw(sprite);
            fenetre.display();
        }
    }
    fenetre.close();

    if (a_fichier_dest) {
        // std::cout << "Saving file to " << fichier_dest << std::endl;
        rtMoteur.sauvegarderDocumentXml(fichier_dest);
    }
    if (sauvegarder_image) {
        // std::cout << "Saving image to " << fichier_image_dest << std::endl;
        rtMoteur.getTexture().copyToImage().saveToFile(fichier_image_dest);
    }

    terminal.fermer();


    return 0;
}
