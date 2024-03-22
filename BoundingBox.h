#ifndef BOUNDINGBOX_H_INCLUDED
#define BOUNDINGBOX_H_INCLUDED
#include "rt.h"

class BoundingBox {
private:
    point mini;
    point maxi;
public:
    BoundingBox() {}
    BoundingBox(const point& m, const point& n) { mini = m; maxi = n;}

    point3 min() const { return mini; }
    point3 max() const { return maxi; }

    void setMin(const point3& m) { this->mini = m; }
    void setMax(const point3& m) { this->maxi = m; }


   bool touche(const rayon& r, double mi, double ma) const {
    for (int i = 0; i < 3; i++) {
        double inverseDi = 1.0 / r.direction()[i];
        double a = (mini[i] - r.origine()[i]) * inverseDi;
        double b = (maxi[i] - r.origine()[i]) * inverseDi;

        if (inverseDi < 0.0) {
            double t = a;
            a = b;
            b = t;
        }

        if (a > mi) {
            mi = a;
        }

        if (b< ma) {
            ma = b;
        }

        if (ma<= mi) {
            return false;
        }
    }
    return true;
}
friend BoundingBox creer_surrounding_box(const BoundingBox& a, const BoundingBox& b);


};

inline double minimum(double a, double b) {
    return (a < b) ? a : b;
}

inline double maximum(double a, double b) {
    return (a > b) ? a : b;
}


BoundingBox creer_surrounding_box(const BoundingBox& b0, const BoundingBox& b1) {
   point p(minimum(b0.mini.x(), b1.mini.x()),
        minimum(b0.mini.y(), b1.mini.y()),
        minimum(b0.mini.z(), b1.mini.z()));

   point g(maximum(b0.maxi.x(), b1.maxi.x()),
        maximum(b0.maxi.y(), b1.maxi.y()),
        maximum(b0.maxi.z(), b1.maxi.z()));

    return BoundingBox(p, g);
}

#endif // BOUNDINGBOX_H_INCLUDED
