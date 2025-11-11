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
    static double cross(const Point &p1, const Point &p2) {
        return p1.getX()*p2.getY() - p1.getY()*p2.getX();
    }

    static bool pointInConvexPolygon(const std::vector<Point> &poly, const Point &p) {
        const int n = static_cast<int>(poly.size());
        if (n == 0) return false;

        if (n < 3) return false;

        const Point &A = poly[0];

        if (cross(poly[1]-A, p-A) < -EPS) return false;
        if (cross(poly[n-1]-A, p-A) > EPS) return false;

        int left = 1, right = n-1;
        while (right - left > 1) {
            int mid = (left + right) / 2;
            if (cross(poly[mid]-A, p-A) > 0) left = mid;
            else right = mid;
        }

        const double c = cross(poly[left]-A, poly[right]-A);
        const double c1 = cross(poly[left]-A, p-A);
        const double c2 = cross(p-A, poly[right]-A);

        return c1 >= -EPS && c2 >= -EPS && (c - c1 - c2) >= -EPS;
    }

    static double haversineDistance(const Point& p1, const Point& p2) {
        constexpr double R = 6371.0;

        const double lat1 = p1.getY() * M_PI / 180.0;
        const double lon1 = p1.getX() * M_PI / 180.0;
        const double lat2 = p2.getY() * M_PI / 180.0;
        const double lon2 = p2.getX() * M_PI / 180.0;

        const double dLat = lat2 - lat1;
        const double dLon = lon2 - lon1;

        const double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                         std::cos(lat1) * std::cos(lat2) *
                         std::sin(dLon / 2) * std::sin(dLon / 2);

        const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

        return R * c;
    }
};


#endif //PROJETOCONCLUSAOCURSO_POINT_HELPER_H