//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_GRID_HELPER_H
#define PROJETOCONCLUSAOCURSO_GRID_HELPER_H
#include <vector>

#include "./PointHelper.h"
#include "../geometry/Cell.h"
#include "../geometry/Polygon.h"
#include "../geometry/UniformGrid.h"


class GridHelper {
public:
    static std::vector<Cell> getOccupiedCells(const std::vector<Polygon> &polygons, const UniformGrid &grid) {
        std::vector<Cell> occupied;

        for (auto &polygon : polygons) {
            double minX = 1e18, minY = 1e18;
            double maxX = -1e18, maxY = -1e18;

            for (const auto &p : polygon.getPoints()) {
                minX = std::min(minX, p.getX());
                minY = std::min(minY, p.getY());
                maxX = std::max(maxX, p.getX());
                maxY = std::max(maxY, p.getY());
            }

            const int iMin = static_cast<int>(std::floor(minX / grid.getCellSize()));
            const int jMin = static_cast<int>(std::floor(minY / grid.getCellSize()));
            const int iMax = static_cast<int>(std::floor(maxX / grid.getCellSize()));
            const int jMax = static_cast<int>(std::floor(maxY / grid.getCellSize()));

            for (int i = iMin; i <= iMax; i++) {
                for (int j = jMin; j <= jMax; j++) {
                    Cell c(i, j);

                    const double x0 = i * grid.getCellSize();
                    const double y0 = j * grid.getCellSize();
                    const double x1 = x0 + grid.getCellSize();
                    const double y1 = y0 + grid.getCellSize();

                    std::vector cellPoly = {
                        Point(-1, x0, y0),
                        Point(-1, x1, y0),
                        Point(-1, x1, y1),
                        Point(-1, x0, y1)
                    };

                    bool intersects = false;

                    for (size_t k = 0; k < polygon.getPoints().size(); k++) {
                        Point a = polygon.getPoints()[k];
                        Point b = polygon.getPoints()[(k + 1) % polygon.getPoints().size()];

                        for (size_t m = 0; m < cellPoly.size(); m++) {
                            Point u = cellPoly[m];
                            Point v = cellPoly[(m + 1) % cellPoly.size()];

                            if (PointHelper::segIntersects(a, b, u, v)) {
                                intersects = true;
                                break;
                            }
                        }
                        if (intersects) break;
                    }

                    if (!intersects) {
                        for (const auto &p : cellPoly) {
                            if (PointHelper::pointInPolygon(polygon.getPoints(), p)) {
                                intersects = true;
                                break;
                            }
                        }
                    }

                    if (!intersects) {
                        for (const auto &p : polygon.getPoints()) {
                            if (p.getX() >= x0 && p.getX() <= x1 &&
                                p.getY() >= y0 && p.getY() <= y1) {
                                intersects = true;
                                break;
                                }
                        }
                    }

                    if (intersects) {
                        occupied.push_back(c);
                    }
                }
            }
        }

        return occupied;
    }
};


#endif //PROJETOCONCLUSAOCURSO_GRID_HELPER_H