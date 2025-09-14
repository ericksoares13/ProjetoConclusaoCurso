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
    Point center;

public:
    Polygon();
    explicit Polygon(const std::vector<Point>& points);

    const std::vector<Point>& getPoints() const;

    bool updatePosition(double dx, double dy, UniformGrid &grid);

    static Polygon generateHexInGrid(UniformGrid &grid, double hexRadius);
};


#endif //PROJETOCONCLUSAOCURSO_POLYGON_H