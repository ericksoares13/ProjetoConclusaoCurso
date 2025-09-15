//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_GRID_HELPER_H
#define PROJETOCONCLUSAOCURSO_GRID_HELPER_H
#include <unordered_set>
#include <vector>

#include "./PointHelper.h"
#include "../geometry/Cell.h"
#include "../geometry/Polygon.h"
#include "../geometry/UniformGrid.h"


class GridHelper {
public:
    static std::vector<Cell> getOccupiedCells(const std::vector<Polygon> &polygons, const UniformGrid &grid) {
        std::unordered_set<Cell, Cell::Hash> occupiedCellsSet;

        for (const auto &polygon : polygons) {
            if (polygon.getPoints().empty()) {
                continue;
            }

            double minX = std::numeric_limits<double>::infinity();
            double maxX = -std::numeric_limits<double>::infinity();
            double minY = std::numeric_limits<double>::infinity();
            double maxY = -std::numeric_limits<double>::infinity();

            for (const auto &p : polygon.getPoints()) {
                minX = std::min(minX, p.getX());
                minY = std::min(minY, p.getY());
                maxX = std::max(maxX, p.getX());
                maxY = std::max(maxY, p.getY());
            }

            const double cellSize = grid.getCellSize();
            const int iMin = static_cast<int>(std::floor(minX / cellSize));
            const int iMax = static_cast<int>(std::floor(maxX / cellSize));
            const int jMin = static_cast<int>(std::floor(minY / cellSize));
            const int jMax = static_cast<int>(std::floor(maxY / cellSize));

            for (int i = iMin; i <= iMax; i++) {
                for (int j = jMin; j <= jMax; j++) {
                    occupiedCellsSet.insert(Cell(i, j));
                }
            }
        }

        return {occupiedCellsSet.begin(), occupiedCellsSet.end()};
    }
};


#endif //PROJETOCONCLUSAOCURSO_GRID_HELPER_H