//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_EDGE_H
#define PROJETOCONCLUSAOCURSO_EDGE_H
#include "Point.h"


class Edge {
    Point* u;
    Point* v;
    double dist;    // Distância em metros

public:
    Edge() : u(nullptr), v(nullptr), dist(0.0) {}
    Edge(Point* u, Point* v, const double dist) : u(u), v(v), dist(dist) {}

    Point* getU() const { return this->u; }
    Point* getV() const { return this->v; }
    double getDist() const { return this->dist; }
};


#endif //PROJETOCONCLUSAOCURSO_EDGE_H