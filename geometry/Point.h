//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_POINT_H
#define PROJETOCONCLUSAOCURSO_POINT_H
#include <cmath>


class Point {
    long long id;
    double x;       // Longitude
    double y;       // Latitude

public:
    Point() : id(-1), x(INFINITY), y(INFINITY) {}
    Point(const long long id, const double x, const double y) : id(id), x(x), y(y) {}

    long long getId() const { return this->id; }
    double getX() const { return this->x; }
    double getY() const { return this->y; }

    void setX(const double newX) { this->x = newX; }
    void setY(const double newY) { this->y = newY; }

    Point operator-(const Point &other) const {
        return {-1, this->x - other.x , this->y - other.y};
    }

    Point operator*(const double k) const {
        return {-1, this->x*k , this->y*k};
    }
};


#endif //PROJETOCONCLUSAOCURSO_POINT_H