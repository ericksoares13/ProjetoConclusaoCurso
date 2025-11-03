//
// Created by erick on 14/09/2025.
//

#include "Polygon.h"

#include <random>

Polygon::Polygon() = default;

Polygon::Polygon(const std::vector<Point> &points) : points(points) {}

bool Polygon::updatePosition(const double dx, const double dy, const UniformGrid &grid) {
    const double newCenterX = this->center.getX() + dx;
    const double newCenterY = this->center.getY() + dy;

    const int cellI = static_cast<int>(floor(newCenterX / grid.getCellSize()));
    const int cellJ = static_cast<int>(floor(newCenterY / grid.getCellSize()));

    if (const Cell newCell(cellI, cellJ); !grid.getGrid().contains(newCell)) {
        return false;
    }

    this->center.setX(newCenterX);
    this->center.setY(newCenterY);

    for (auto &p : this->points) {
        p.setX(p.getX() + dx);
        p.setY(p.getY() + dy);
    }

    return true;
}

Polygon Polygon::generateHexInGrid(const UniformGrid &grid, const double hexRadius) {
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

    std::uniform_real_distribution<> distX(x0 + maxMoveDistance, x0 + grid.getCellSize() - maxMoveDistance);
    std::uniform_real_distribution<> distY(y0 + maxMoveDistance, y0 + grid.getCellSize() - maxMoveDistance);

    const double centerX = distX(gen);
    const double centerY = distY(gen);

    Polygon hex;
    hex.center = Point(-1, centerX, centerY);

    for (int k = 0; k < 6; k++) {
        const double angle = M_PI / 3.0 * k;
        const double x = centerX + hexRadius * cos(angle);
        const double y = centerY + hexRadius * sin(angle);
        hex.points.emplace_back(k, x, y);
    }

    return hex;
}
