#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "rt.h"
#include "../include/tinyxml2.h"
#include <iostream>
class camera {

private:
    Point viewerPosition;
    Point lowerLeft;
    vecteur3 horizontal;
    vecteur3 vertical;
    vecteur3 u, v, w;
    double lensDiameter;
    double startTime, endTime;

    // Parameters storage
    Point gazeAt;
    vecteur3 verticalUp;
    double verticalFieldOfView, aspectRatio, apertureSize, focalDistance;

public:
    camera() {};

    camera(
        Point observerPosition,
        Point gazeAt,
        vecteur3 verticalUp,
        double verticalFieldOfView,
        double aspectRatio,
        double apertureSize,
        double focalDistance,
        double _startTime = 0,
        double _endTime = 0) : gazeAt(gazeAt), verticalUp(verticalUp),
        verticalFieldOfView(verticalFieldOfView), aspectRatio(aspectRatio),
        apertureSize(apertureSize), focalDistance(focalDistance), startTime(_startTime),
        endTime(_endTime) {

        auto theta = degreesToRadians(verticalFieldOfView);
        auto h = tan(theta / 2);
        auto viewportHeight = 2.0 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        w = vecteur_unitaire(observerPosition - gazeAt);
        u = vecteur_unitaire(produit_vecteur3iel(verticalUp, w));
        v = produit_vecteur3iel(w, u);

        viewerPosition = observerPosition;

        horizontal = focalDistance * viewportWidth * u;
        vertical = focalDistance * viewportHeight * v;
        lowerLeft = viewerPosition - horizontal / 2 - vertical / 2 - focalDistance * w;

        lensDiameter = apertureSize / 2;
        startTime = _startTime;
        endTime = _endTime;
    }

    camera(tinyxml2::XMLElement * pElement) {
        apertureSize = pElement->DoubleAttribute("Aperture");
        verticalFieldOfView = pElement->DoubleAttribute("VerticalFieldOfView");
        aspectRatio = pElement->DoubleAttribute("AspectRatio");
        focalDistance = pElement->DoubleAttribute("FocalDistance");
        startTime = pElement->DoubleAttribute("StartTime");
        endTime = pElement->DoubleAttribute("EndTime");

        tinyxml2::XMLElement * pObserverPosElement = pElement->FirstChildElement("ObserverPosition");
        if (pObserverPosElement == nullptr) throw std::invalid_argument("Vision Device Element does not have an ObserverPosition element");

        viewerPosition = vecteur3(pObserverPosElement);

        tinyxml2::XMLElement * pGazeAtElement = pElement->FirstChildElement("GazeAt");
        if (pGazeAtElement == nullptr) throw std::invalid_argument("Vision Device Element does not have a GazeAt element");

        gazeAt = vecteur3(pGazeAtElement);

        tinyxml2::XMLElement * pVerticalUpElement = pElement->FirstChildElement("VerticalUp");
        if (pVerticalUpElement == nullptr) throw std::invalid_argument("Vision Device Element does not have a VerticalUp element");

        verticalUp = vecteur3(pVerticalUpElement);

        auto theta = degreesToRadians(verticalFieldOfView);
        auto h = tan(theta / 2);
        auto viewportHeight = 2.0 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        w = vecteur_unitaire(viewerPosition - gazeAt);
        u = vecteur_unitaire(produit_vectoriel(verticalUp, w));
        v = produit_vectoriel(w, u);

        horizontal = focalDistance * viewportWidth * u;
        vertical = focalDistance * viewportHeight * v;
        lowerLeft = viewerPosition - horizontal / 2 - vertical / 2 - focalDistance * w;

        lensDiameter = apertureSize / 2;
    }

    rayon getrayon(double s, double t) const {
        vecteur3 rd = lensDiameter * randomInUnitDisk();
        vecteur3 offset = u * rd.x() + v * rd.y();

        return rayon(
            viewerPosition + offset,
            lowerLeft + s * horizontal + t * vertical - viewerPosition - offset,
            randomDouble(startTime, endTime)
        );
    }

    tinyxml2::XMLElement* toXml(tinyxml2::XMLDocument& xmlDoc) const {
        tinyxml2::XMLElement * pElement = xmlDoc.NewElement("camera");

        pElement->SetAttribute("Aperture", apertureSize);
        pElement->SetAttribute("VerticalFieldOfView", verticalFieldOfView);
        pElement->SetAttribute("AspectRatio", aspectRatio);
        pElement->SetAttribute("FocalDistance", focalDistance);
        pElement->SetAttribute("StartTime", startTime);
        pElement->SetAttribute("EndTime", endTime);

        tinyxml2::XMLElement* observerPosXml = xmlDoc.NewElement("ObserverPosition");
        viewerPosition.toXml(observerPosXml);
        pElement->InsertEndChild(observerPosXml);

        tinyxml2::XMLElement* gazeAtXml = xmlDoc.NewElement("GazeAt");
        gazeAt.toXml(gazeAtXml);
        pElement->InsertEndChild(gazeAtXml);

        tinyxml2::XMLElement* verticalUpXml = xmlDoc.NewElement("VerticalUp");
        verticalUp.toXml(verticalUpXml);
        pElement->InsertEndChild(verticalUpXml);

        return pElement;
    }
};



#endif // CAMERA_H_INCLUDED
