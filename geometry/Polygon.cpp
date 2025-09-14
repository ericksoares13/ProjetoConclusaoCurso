//
// Created by erick on 14/09/2025.
//

#include "Polygon.h"

#include <random>

Polygon::Polygon() = default;

Polygon::Polygon(const std::vector<Point> &vertices) : vertices(vertices) {}

std::vector<Point>& Polygon::getVertices() {
    return this->vertices;
}

Polygon Polygon::generateHexInGrid(UniformGrid &grid, const double hexRadius) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<Cell> nonEmptyCells;
    for (auto &[cell, edges] : grid.getGrid()) {
        if (!edges.empty()) {
            nonEmptyCells.push_back(cell);
        }
    }

    if (nonEmptyCells.empty()) {
        return {};
    }

    std::uniform_int_distribution<> distCell(0, static_cast<int>(nonEmptyCells.size() - 1));
    const Cell chosenCell = nonEmptyCells[distCell(gen)];

    const double x0 = chosenCell.getI() * grid.getCellSize();
    const double y0 = chosenCell.getJ() * grid.getCellSize();

    std::uniform_real_distribution<> distX(x0, x0 + grid.getCellSize());
    std::uniform_real_distribution<> distY(y0, y0 + grid.getCellSize());

    const double centerX = distX(gen);
    const double centerY = distY(gen);

    Polygon hex;
    for (int k = 0; k < 6; k++) {
        const double angle = M_PI / 3.0 * k;
        const double x = centerX + hexRadius * cos(angle);
        const double y = centerY + hexRadius * sin(angle);
        hex.vertices.emplace_back(k, x, y);
    }

    return hex;
}
