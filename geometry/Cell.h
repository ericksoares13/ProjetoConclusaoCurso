//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_CELL_H
#define PROJETOCONCLUSAOCURSO_CELL_H
#include <functional>


class Cell {
    int i;
    int j;

public:
    Cell() : i(0), j(0) {}
    Cell(const int i, const int j) : i(i), j(j) {}
    ~Cell() = default;

    int getI() const { return this->i; }
    int getJ() const { return this->j; }

    bool operator==(const Cell& other) const {
        return this->i == other.i && this->j == other.j;
    }

    class Hash {
    public:
        std::size_t operator()(const Cell& c) const {
            return std::hash<int>()(c.i) ^ (std::hash<int>()(c.j) << 1);
        }
    };
};


#endif //PROJETOCONCLUSAOCURSO_CELL_H