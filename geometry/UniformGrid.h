//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_UNIFORM_GRID_H
#define PROJETOCONCLUSAOCURSO_UNIFORM_GRID_H
#include <vector>

#include "Cell.h"
#include "Edge.h"


class UniformGrid {
    std::unordered_map<Cell, std::vector<Edge *>, Cell::Hash> grid;
    double cellSize;

public:
    UniformGrid() : cellSize(0.01) {}
    explicit UniformGrid(const double cellSize) : cellSize(cellSize) {}

    const std::unordered_map<Cell, std::vector<Edge *>, Cell::Hash> &getGrid() const { return this->grid; }
    double getCellSize() const { return this->cellSize; }

    void insertEdge(Edge *edge) {
        const int iMin = floor(std::min(edge->getU()->getX(), edge->getV()->getX()) / this->cellSize);
        const int iMax = floor(std::max(edge->getU()->getX(), edge->getV()->getX()) / this->cellSize);
        const int jMin = floor(std::min(edge->getU()->getY(), edge->getV()->getY()) / this->cellSize);
        const int jMax = floor(std::max(edge->getU()->getY(), edge->getV()->getY()) / this->cellSize);

        for (int i = iMin; i <= iMax; i++) {
            for (int j = jMin; j <= jMax; j++) {
                this->grid[Cell(i,j)].push_back(edge);
            }
        }
    }
};


#endif //PROJETOCONCLUSAOCURSO_UNIFORM_GRID_H