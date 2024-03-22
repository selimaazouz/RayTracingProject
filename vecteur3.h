#ifndef VECTEUR3_H_INCLUDED
#define VECTEUR3_H_INCLUDED
#include <cmath>
#include <iostream>
#include <random>
#include "../include/tinyxml2.h"

#include "rt.h"

class vecteur3 {
private:
    double v[3];

public:
    vecteur3() : v{0, 0, 0} {}

    vecteur3(double x, double y, double z) : v{x, y, z} {}
    double x() const { return v[0]; }

    double y() const { return v[1]; }

    double z() const { return v[2]; }
    void setX(double value) { v[0] = value; }
    void setY(double value) { v[1] = value; }
    void setZ(double value) { v[2] = value; }

    vecteur3(tinyxml2::XMLElement* pElement) {
        v[0] = pElement->DoubleAttribute("x");
        v[1] = pElement->DoubleAttribute("y");
        v[2] = pElement->DoubleAttribute("z");
    }

     double operator[](int i) const { return v[i]; }

    double& operator[](int i) { return v[i]; }
    vecteur3& operator+=(const vecteur3& w) {
        v[0] += w.v[0];
        v[1] += w.v[1];
        v[2] += w.v[2];
        return *this;
    }

    vecteur3 operator-() const { return vecteur3(-v[0], -v[1], -v[2]); }



    vecteur3& operator*=(const double t) {
        v[0] *= t;
        v[1] *= t;
        v[2] *= t;
        return *this;
    }

    vecteur3& operator/=(const double t) {
        return *this *= 1 / t;
    }

    double norme() const {
        return sqrt(norme2());
    }

    double norme2() const {
        return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    }

    inline static vecteur3 random() {
        return vecteur3(random_double(), random_double(), random_double());
    }

    inline static vecteur3 random(double min, double max) {
        return vecteur3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    bool proche_de_zero() const {
        const auto s = 1e-8;
        return (fabs(v[0]) < s) && (fabs(v[1]) < s) && (fabs(v[2]) < s);
    }

    void to_xml(tinyxml2::XMLElement* pElement) const {
        pElement->SetAttribute("x", x());
        pElement->SetAttribute("y", y());
        pElement->SetAttribute("z", z());
    }


    friend std::ostream& operator<<(std::ostream& flux, const vecteur3& w);
    friend vecteur3 operator+(const vecteur3& u, const vecteur3& w);
    friend vecteur3 operator-(const vecteur3& u, const vecteur3& w);
    friend vecteur3 operator*(const vecteur3& u, const vecteur3& w);
    friend vecteur3 operator*(double t, const vecteur3& w);
    friend vecteur3 operator*(const vecteur3& w, double t);
    friend vecteur3 operator/(vecteur3 w, double t);
    friend double produit_scalaire(const vecteur3& u, const vecteur3& w);
    friend vecteur3 produit_vectoriel(const vecteur3& u, const vecteur3& w);
    friend vecteur3 vecteur_unitaire(vecteur3 w);
    friend vecteur3 refleter(const vecteur3& w, const vecteur3& n);
    friend vecteur3 refracter(const vecteur3& uw, const vecteur3& n, double etai_over_etat);
    friend vecteur3 point_aleatoire_dans_sphere();
    friend vecteur3 vecteur_unitaire_aleatoire();
    friend vecteur3 point_aleatoire_dans_hemisphere(const vecteur3& normal);
    friend vecteur3 point_aleatoire_dans_disque();
};


std::ostream& operator<<(std::ostream& flux, const vecteur3& w) {
    return flux << w.e[0] << ' ' << w.e[1] << ' ' << w.e[2];
}

vecteur3 operator+(const vecteur3& u, const vecteur3& w) {
    return vecteur3(u.e[0] + w.e[0], u.e[1] + w.e[1], u.e[2] + w.e[2]);
}

vecteur3 operator-(const vecteur3& u, const vecteur3& w) {
    return vecteur3(u.e[0] - w.e[0], u.e[1] - w.e[1], u.e[2] - w.e[2]);
}

vecteur3 operator*(const vecteur3& u, const vecteur3& w) {
    return vecteur3(u.e[0] * w.e[0], u.e[1] * w.e[1], u.e[2] * w.e[2]);
}

vecteur3 operator*(double t, const vecteur3& w) {
    return vecteur3(t * w.e[0], t * w.e[1], t * w.e[2]);
}

vecteur3 operator*(const vecteur3& w, double t) {
    return t * w;
}

vecteur3 operator/(vecteur3 w, double t) {
    return (1 / t) * w;
}

double produit_scalaire(const vecteur3& u, const vecteur3& w) {
    return u.e[0] * w.e[0] + u.e[1] * w.e[1] + u.e[2] * w.e[2];
}

vecteur3 produit_vectoriel(const vecteur3& u, const vecteur3& w) {
    return vecteur3(u.e[1] * w.e[2] - u.e[2] * w.e[1], u.e[2] * w.e[0] - u.e[0] * w.e[2], u.e[0] * w.e[1] - u.e[1] * w.e[0]);
}

vecteur3 vecteur_unitaire(vecteur3 w) {
    return w / w.norme();
}

vecteur3 refleter(const vecteur3& w, const vecteur3& n) {
    return w - 2 * produit_scalaire(w, n) * n;
}

vecteur3 refracter(const vecteur3& uw, const vecteur3& n, double indice_refraction_ratio) {
    auto cos = fmin(produit_scalaire(-uw, n), 1.0);
    vecteur3 refracted_perpendicular = indice_refraction_ratio * (uw + cos * n);
    vecteur3 refracted_parallel = -sqrt(fabs(1.0 - refracted_perpendicular.norme2())) * n;
    return refracted_perpendiculars + refracted_parallel;
}

vecteur3 point_aleatoire_dans_sphere() {
    while (true) {
        auto p = vecteur3::random(-1, 1);
        if (p.norme2() < 1)
            return p;
    }
}


vecteur3 vecteur_unitaire_aleatoire() {
    return vecteur_unitaire(point_aleatoire_dans_sphere());
}

vecteur3 point_aleatoire_dans_hemisphere(const vecteur3& normal) {
    vecteur3 dans_sphere = point_aleatoire_dans_sphere();
    return produit_scalaire(dans_sphere, normal) > 0.0 ? dans_sphere : -dans_sphere;
}

vecteur3 point_aleatoire_dans_disque() {
    while (true) {
        auto p = vecteur3(aleatoire_double(-1, 1), aleatoire_double(-1, 1), 0);
        if (p.norme2() < 1)
            return p;
    }
}


using point = vecteur3;
using couleur = vecteur3;
#endif // vecteur3_H
