//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_POINT_HELPER_H
#define PROJETOCONCLUSAOCURSO_POINT_HELPER_H
#include <vector>

#include "../geometry/Edge.h"
#include "../geometry/Point.h"

#define EPS 1e-9


class PointHelper {
public:
    static double inner(const Point &p1, const Point &p2) {
        return p1.getX()*p2.getX() + p1.getY()*p2.getY();
    }

    static double cross(const Point &p1, const Point &p2) {
        return p1.getX()*p2.getY() - p1.getY()*p2.getX();
    }

    static bool collinear(const Point &p, const Point &q, const Point &r) {
        return fabs(cross(p-q, r-p)) < EPS;
    }

    static bool between(const Point &p, const Point &q, const Point &r) {
        return collinear(p, q, r) && inner(p-q, r-q) <= 0;
    }

    static Point lineIntersectSeg(const Point &p, const Point &q, const Point &U, const Point &V) {
        const double c = cross(U-V, p-q);
        const double a = cross(U, V);
        const double b = cross(p, q);
        return ((p-q)*(a/c)) - ((U-V)*(b/c));
    }

    static bool parallel(const Point &a, const Point &b) {
        return fabs(cross(a ,b)) < EPS;
    }

    bool static segIntersects(const Point &a ,const Point &b ,const Point &p ,const Point &q) {
        if (parallel(a-b, p-q)) {
            return between(a, p, b) || between (a, q, b)
            || between(p, a, q) || between(p, b, q);
        }
        const Point i = lineIntersectSeg(a, b, p, q);
        return between(a, i, b) && between (p, i, q);
    }

    static bool pointInPolygon(const std::vector<Point> &polygon, const Point &p) {
        bool inside = false;
        const int n = static_cast<int>(polygon.size());
        for (int i = 0, j = n - 1; i < n; j = i++) {
            const Point &pi = polygon[i];
            const Point &pj = polygon[j];

            const bool intersect = ((pi.getY() > p.getY()) != (pj.getY() > p.getY())) &&
                             (p.getX() < (pj.getX() - pi.getX()) * (p.getY() - pi.getY()) / (pj.getY() - pi.getY() + EPS) + pi.getX());
            if (intersect) inside = !inside;
        }
        return inside;
    }

    static bool polygonIntersectsOrContainsEdge(const std::vector<Point> &poly, const Edge &edge) {
        const int n = static_cast<int>(poly.size());
        const Point *u = edge.getU();
        const Point *v = edge.getV();

        for (int i = 0; i < n; i++) {
            Point a = poly[i];
            Point b = poly[(i + 1) % n];

            if (segIntersects(a, b, *u, *v)) {
                return true;
            }
        }

        if (pointInPolygon(poly, *u) || pointInPolygon(poly, *v)) {
            return true;
        }

        return false;
    }
};


#endif //PROJETOCONCLUSAOCURSO_POINT_HELPER_H