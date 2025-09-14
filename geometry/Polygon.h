//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_POLYGON_H
#define PROJETOCONCLUSAOCURSO_POLYGON_H
#include <vector>

#include "Point.h"
#include "UniformGrid.h"


class Polygon {
    std::vector<Point> points;

public:
    Polygon();
    explicit Polygon(const std::vector<Point>& points);

    std::vector<Point>& getPoints();

    void updatePosition();

    static Polygon generateHexInGrid(UniformGrid &grid, double hexRadius);
};


#endif //PROJETOCONCLUSAOCURSO_POLYGON_H