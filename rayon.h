#ifndef RAYON_H_INCLUDED
#define RAYON_H_INCLUDED
#include "vecteur3.h"
#include "ObjectList.h"
class rayon {
private:

    vecteur3 origine;
    vecteur3 direction;
    double temps;
public:

    rayon() {}
    rayon(const vecteur3& o, const vecteur3& d, double t = 0.0)
        : origine(o), direction(d), temps(t)
    {}

    vecteur3 origine() const  { return origine; }


    vecteur3 direction() const { return direction; }


    double temps() const    { return temps; }


    vecteur3 pt_a_distance(double distance) const {
        return origine + distance * direction;
    }


};



#endif // RAYON_H_INCLUDED
