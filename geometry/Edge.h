//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_EDGE_H
#define PROJETOCONCLUSAOCURSO_EDGE_H
#include "Point.h"


class Edge {
    Point* u;
    Point* v;
    double dist;
    bool valid;

public:
    Edge() : u(nullptr), v(nullptr), dist(0.0), valid(false) {}
    Edge(Point* u, Point* v, const double dist, const bool valid) : u(u), v(v), dist(dist), valid(valid) {}

    Point* getU() const { return this->u; }
    Point* getV() const { return this->v; }
    double getDist() const { return this->dist; }

    void setValid(const bool newValid) { this->valid = newValid; }

    bool isValid() const { return this->valid; }
};


#endif //PROJETOCONCLUSAOCURSO_EDGE_H