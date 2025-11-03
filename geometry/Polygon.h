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
    double velocityX = 0.0;
    double velocityY = 0.0;

public:
    static constexpr double inertia = 0.8;
    static constexpr double acceleration = 0.5;
    static constexpr double maxMoveDistance = 0.0009;

    Polygon();
    explicit Polygon(const std::vector<Point>& points);

    const std::vector<Point>& getPoints() const { return this->points; };
    double getVelocityX() const { return this->velocityX; };
    double getVelocityY() const { return this->velocityY; };

    void setVelocityX(const double newVelocityX) { this->velocityX = newVelocityX; };
    void setVelocityY(const double newVelocityY) { this->velocityY = newVelocityY; };

    bool updatePosition(double dx, double dy, const UniformGrid &grid);

    static Polygon generateHexInGrid(const UniformGrid &grid, double hexRadius);
};


#endif //PROJETOCONCLUSAOCURSO_POLYGON_H